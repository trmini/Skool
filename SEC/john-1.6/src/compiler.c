/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "arch.h"
#include "params.h"
#include "memory.h"
#include "compiler.h"

char *c_errors[] = {
	NULL,	/* No error */
	"Unknown identifier",
	"Unexpected character",
	"Error in expression",
	"Identifier is too long",
	"Expression is too complex",
	"Invalid array size",
	"Data section is too large",
	"Duplicate identifier",
	"Keyword is used as an identifier",
	"Not in a function",
	"Nested functions are not supported",
	"Not in an if statement",
	"Not in a loop",
	"Unexpected end of source",
	"Internal compiler error"
};

int c_errno;

union c_insn {
	void (*op)();
	c_int *mem;
	c_int imm;
	union c_insn *pc;
};

struct c_fixup {
	struct c_fixup *next;
	union c_insn *pc;
};

static int c_pass;

static union c_insn *c_code_start;
static union c_insn *c_code_ptr;
static union c_insn *c_pc;

static c_int *c_data_start;
static c_int *c_data_ptr;

static union c_insn c_stack[C_STACK_SIZE];
static union c_insn *c_sp;

static union c_insn *c_loop_start;
static struct c_fixup *c_break_fixups;

static struct c_ident *c_funcs;

static char c_unget_buffer[C_UNGET_SIZE];
static int c_unget_count;

static char c_isident[0x100];
#define c_isstart(c) \
	(c_isident[(ARCH_INDEX)(c)] && ((c) < '0' || (c) > '9'))

static int c_EOF;

static int (*c_ext_getchar)();
static void (*c_ext_rewind)();

static char *c_reserved[] = {
	"void",
	"int",
	"if",
	"else",
	"while",
	"continue",
	"break",
	"return",
	NULL
};

#define C_LEFT_TO_RIGHT			0
#define C_RIGHT_TO_LEFT			1

#define C_CLASS_BINARY			0
#define C_CLASS_LEFT			1
#define C_CLASS_RIGHT			2

struct c_op {
	int prec;
	int dir;
	int class;
	char *name;
	void (*op)();
};

#ifdef __GNUC__
static struct c_op c_ops[];
#else
static struct c_op c_ops[38];
#endif

static void c_init()
{
	int c;

	for (c = 0; c < 0x100; c++)
	if (c < 0x80)
		c_isident[c] = (isalpha(c) || isdigit(c) || c == '_') ? 1 : 0;
	else
		c_isident[c] = 0;

	c_code_ptr = c_code_start;
	c_data_ptr = c_data_start;

	c_loop_start = NULL;
	c_break_fixups = NULL;

	c_funcs = NULL;

	c_unget_count = 0;

	c_EOF = 0;
	c_errno = 0;

	c_ext_rewind();
}

static void c_ungetchar(char c)
{
	if (c_unget_count >= C_UNGET_SIZE)
		c_errno = C_ERROR_INTERNAL;
	else
		c_unget_buffer[c_unget_count++] = c;
}

static char c_buffer_getchar()
{
	int c;

	if (c_unget_count) return c_unget_buffer[--c_unget_count];
	if ((c = c_ext_getchar()) > 0) return c;

	c_EOF = 1;
	c_errno = C_ERROR_EOF;
	return ' ';
}

static char c_getchar(int quote)
{
	int c;
	int space = 0;

	do {
		c = (unsigned char)c_buffer_getchar();
		if (quote || c_EOF) return c;

		if (c <= ' ') space = 1; else
		if (c == '/')
		switch ((c = c_buffer_getchar())) {
		case '/':
			do {
				c = c_buffer_getchar();
			} while (!c_EOF && c != '\n' && c != '\r');
			c = ' '; space = 1;
			break;

		case '*':
			do {
				if ((c = c_buffer_getchar()) == '*')
				if ((c = c_buffer_getchar()) == '/')
					break;
			} while (!c_EOF);
			c = ' '; space = 1;
			break;

		default:
			c_ungetchar(c);
			c = '/';
		}
	} while (c <= ' ');

	if (space) {
		c_ungetchar(c);
		c = ' ';
	}

	return c;
}

static char *c_gettoken()
{
	static char token[C_TOKEN_SIZE];
	int pos = 0;

	while (c_isident[(ARCH_INDEX)(token[pos++] = c_getchar(0))])
	if (pos >= C_TOKEN_SIZE) {
		c_errno = C_ERROR_TOOLONG;
		break;
	}

	if (pos != 1) c_ungetchar(token[--pos]);
	token[pos] = 0;

	return token;
}

static int c_getint(char *token)
{
	int value;
	char *format;

	if (token[0] == '\'') {
		if ((value = c_getchar(1)) == '\'')
			c_errno = C_ERROR_UNEXPECTED;
		else
			if (value == '\\') value = c_getchar(1);
		if (c_getchar(1) != '\'') c_errno = C_ERROR_UNEXPECTED;
	} else {
		if (token[0] != '0') format = "%d"; else
		if (token[1] != 'x') format = "%o"; else {
			format = "%x";
			token += 2;
		}

		if (sscanf((char *)token, format, &value) != 1)
			c_errno = C_ERROR_UNEXPECTED;
	}

	return value;
}

static char c_skip_space()
{
	char c;

	if ((c = c_getchar(0)) == ' ') c = c_getchar(0);

	return c;
}

static int c_expect(char expected)
{
	char c;

	if ((c = c_getchar(0)) == ' ')
	if (expected != ' ') c = c_getchar(0);

	if (c != expected) c_errno = C_ERROR_UNEXPECTED;

	return c_errno;
}

static struct c_ident *c_find_ident(struct c_ident *list,
	struct c_ident *globals, char *name)
{
	struct c_ident *current;

	if ((current = list) != globals)
	do {
		if (!strcmp(name, current->name)) break;
	} while ((current = current->next) != globals);

	if (current != globals)
		return current;
	else
		return NULL;
}

static int c_alloc_ident(struct c_ident **list, struct c_ident *globals,
	char *name, void *addr)
{
	char **current;
	struct c_ident *last;

	current = c_reserved;
	do {
		if (!strcmp(name, *current)) return c_errno = C_ERROR_RESERVED;
	} while (*++current);

	if (c_find_ident(*list, globals, name)) return c_errno = C_ERROR_DUPE;

	last = *list;
	*list = (struct c_ident *)mem_alloc(sizeof(struct c_ident));
	(*list)->next = last;
	strcpy((*list)->name = (char *)mem_alloc(strlen(name) + 1), name);
	(*list)->addr = addr;

	return c_errno;
}

static void c_free_ident(struct c_ident *list, struct c_ident *globals)
{
	struct c_ident *current;

	while ((current = list) != globals) {
		list = list->next;
		mem_free((void **)&current->name);
		mem_free((void **)&current);
	}
}

static int c_find_op(char *token, int left)
{
	int best = -1;
	int op = 0;

	do {
		if ((c_ops[op].class != C_CLASS_LEFT && left) ||
		    (c_ops[op].class == C_CLASS_LEFT && !left))
		if (!memcmp(c_ops[op].name, token, strlen(c_ops[op].name)))
		if (best < 0 ||
		    strlen(c_ops[op].name) > strlen(c_ops[best].name))
			best = op;
	} while (c_ops[++op].prec);

	return best;
}

static void c_free_fixup(struct c_fixup *list, union c_insn *pc)
{
	struct c_fixup *current;

	while ((current = list)) {
		if (c_pass)
			current->pc->pc = pc;

		list = list->next;
		mem_free((void **)&current);
	}
}

static void (*c_op_return)();
static void (*c_op_bz)();
static void (*c_op_ba)();
static void (*c_op_push_imm)();
static void (*c_op_push_mem)();
static void (*c_op_pop)();

static void (*c_op_push_imm_imm)();
static void (*c_op_push_imm_mem)();
static void (*c_op_push_mem_imm)();
static void (*c_op_push_mem_mem)();

static void (*c_push(void (*last)(), void (*op)(), union c_insn *value))()
{
	if (last == c_op_push_imm || last == c_op_push_mem) {
		if (last == c_op_push_imm) {
			if (op == c_op_push_imm)
				last = c_op_push_imm_imm;
			else
				last = c_op_push_imm_mem;
		} else {
			if (op == c_op_push_imm)
				last = c_op_push_mem_imm;
			else
				last = c_op_push_mem_mem;
		}

		if (c_pass) {
			(c_code_ptr - 2)->op = last;
			*c_code_ptr = *value;
		}
		c_code_ptr++;
	} else {
		last = op;

		if (c_pass) {
			(c_code_ptr++)->op = op;
			*c_code_ptr++ = *value;
		} else
			c_code_ptr += 2;
	}

	return last;
}

static int c_block(char term, struct c_ident *vars);

static int c_define(char term, struct c_ident **vars, struct c_ident *globals)
{
	char *token;
	char c;
	int size;

	c_expect(' ');
	token = c_gettoken();
	if (!c_isstart(*token)) c_errno = C_ERROR_UNEXPECTED;

	do
	if (*token != ' ') {
		if (!c_isstart(*token)) c_errno = C_ERROR_UNEXPECTED;
		if (c_errno) return c_errno;

		if ((c = c_skip_space()) == '(') {
			if (term) return c_errno = C_ERROR_NESTEDFUNC;

			if (c_alloc_ident(&c_funcs, NULL, token, c_code_ptr))
				return c_errno;

			c_expect(')');
			if (c_expect('{')) return c_errno;

			c_block('}', *vars);

			if (c_pass)
				c_code_ptr->op = c_op_return;
			c_code_ptr++;

			break;
		} else {
			if (c_alloc_ident(vars, globals, token, c_data_ptr++))
				return c_errno;

			if (c == '[') {
				size = c_getint(c_gettoken());
				if (c_errno) return c_errno;

				if (size < 1 || size > C_ARRAY_SIZE)
					return c_errno = C_ERROR_ARRAYSIZE;

				c_data_ptr += size - 1;

				if (c_data_ptr - c_data_start > C_DATA_SIZE)
					return c_errno = C_ERROR_DATASIZE;

				c_expect(']');
				c = c_skip_space();
			}

			if (c == ';') break;
			if (c != ',') c_errno = C_ERROR_UNEXPECTED;
		}
	} while (!c_errno && *(token = c_gettoken()) != ';');

	return c_errno;
}

static int c_expr(char term, struct c_ident *vars, char *token)
{
	char c;
	struct c_ident *var;
	int lookahead, op;
	struct c_op *op1, *op2;
	union c_insn value;
	int stack[C_EXPR_SIZE];
	int sp = 0;
	int balance = -1;
	int left = 0;
	void (*last)() = (void (*)())0;

	if (term == ')') stack[sp++] = -1;
	do {
		c = *token;

		if (c == ')' || c == ']' || c == ';' || c == term) {
			while (sp) {
				if (stack[--sp] < 0) break;
				if (c_ops[stack[sp]].class == C_CLASS_BINARY)
					balance--;

				last = c_ops[stack[sp]].op;
				if (c_pass)
					c_code_ptr->op = last;
				c_code_ptr++;

				if (!stack[sp]) break;
			}

			if ((c == ')' && stack[sp] >= 0) ||
			    (c == ']' && stack[sp]) ||
			    ((c == ';' || (term != ')' && c == term)) && sp))
				c_errno = C_ERROR_COUNT;
			if (c_errno || (!sp && c == term)) break;

			left = 1;
		} else
		if ((c >= '0' && c <= '9') || c == '\'') {
			value.imm = c_getint(token);
			last = c_push(last, c_op_push_imm, &value);

			left = 1; balance++;
		} else
		if (c == '(' || c == '[') {
			if (sp >= C_EXPR_SIZE)
				c_errno = C_ERROR_TOOCOMPLEX;
			else
				stack[sp++] = (c == '(') ? -1 : 0;

			left = 0;
		} else
		if (c != ' ') {
			if (c_isident[(ARCH_INDEX)c])
				var = c_find_ident(vars, NULL, token);
			else
				var = NULL;

			if (var) {
				value.mem = var->addr;
				last = c_push(last, c_op_push_mem, &value);

				left = 1; balance++;
			} else {
				if ((lookahead = !token[1])) {
					token[1] = c_getchar(0);
					token[2] = c_getchar(0);
					token[3] = 0;
				}

				if ((op = c_find_op(token, left)) < 0) {
					if (c_isident[(ARCH_INDEX)c])
						c_errno = C_ERROR_UNKNOWN;
					else
						c_errno = C_ERROR_UNEXPECTED;
					return c_errno;
				}

				if (lookahead)
				if (strlen(c_ops[op].name) < 3) {
					c_ungetchar(token[2]);
					if (!c_ops[op].name[1])
						c_ungetchar(token[1]);
				}

				op1 = &c_ops[op];
				while (sp && stack[sp - 1] >= 0) {
					op2 = &c_ops[stack[sp - 1]];

					if (op2->dir == C_RIGHT_TO_LEFT)
					if (op2->prec <= op1->prec) break;

					if (op2->dir == C_LEFT_TO_RIGHT)
					if (op2->prec < op1->prec) break;

					if (op2->class == C_CLASS_BINARY)
						balance--;

					last = op2->op;
					if (c_pass)
						c_code_ptr->op = last;
					c_code_ptr++;

					sp--;
				}

				if (sp >= C_EXPR_SIZE)
					c_errno = C_ERROR_TOOCOMPLEX;
				else {
					stack[sp++] = op;

					left = op1->class == C_CLASS_RIGHT;
				}
			}
		}

		if (c_errno || c == ';' || (c == term && c != ')')) break;
		token = c_gettoken();
	} while (!c_errno);

	if (c_errno) return c_errno;

	if (sp || balance) c_errno = C_ERROR_COUNT;

	if (c_pass)
		c_code_ptr->op = c_op_pop;
	c_code_ptr++;

	if (!term && !c_errno) c_errno = C_ERROR_NOTINFUNC;
	if (*token == term) return -1;

	return c_errno;
}

static int c_cond(char term, struct c_ident *vars, char *token)
{
	char c;
	char *pos;
	union c_insn *start, *out, *fixup;

	if (!term) return c_errno = C_ERROR_NOTINFUNC;

	c = *token;
	start = c_code_ptr;

	if (c_expect('(')) return c_errno;
	switch (c_expr(')', vars, c_gettoken())) {
	case -1:
		break;

	case 0:
		c_errno = C_ERROR_UNEXPECTED;

	default:
		return c_errno;
	}

	if (c_pass)
		(c_code_ptr - 1)->op = c_op_bz;
	fixup = c_code_ptr++;

	out = c_loop_start;
	if (c == 'w')
		c_loop_start = start;

	if (c_block(';', vars)) return c_errno;

	if (c == 'w') {
		c_loop_start = out;

		if (c_pass) {
			(c_code_ptr++)->op = c_op_ba;
			(c_code_ptr++)->pc = start;
		} else
			c_code_ptr += 2;

		c_free_fixup(c_break_fixups, c_code_ptr);
		c_break_fixups = NULL;
	} else {
		while (*(token = c_gettoken()) == ' ')
		if (c_errno) return c_errno;

		if (!strcmp(token, "else")) {
			if (c_pass) {
				(c_code_ptr++)->op = c_op_ba;
				fixup->pc = c_code_ptr + 1;
				fixup = c_code_ptr++;
			} else
				c_code_ptr += 2;

			if (c_block(';', vars)) return c_errno;
		} else {
			pos = token + strlen(token);
			while (pos > token) c_ungetchar(*--pos);
		}
	}

	if (c_pass)
		fixup->pc = c_code_ptr;
	c_ungetchar(';');

	return c_errno;
}

static int c_continue()
{
	if (!c_loop_start) return c_errno = C_ERROR_NOTINLOOP;

	if (c_pass) {
		(c_code_ptr++)->op = c_op_ba;
		(c_code_ptr++)->pc = c_loop_start;
	} else
		c_code_ptr += 2;

	c_expect(';');
	c_ungetchar(';');

	return c_errno;
}

static int c_break(char term, struct c_ident *vars)
{
	struct c_fixup *fixup;

	if (!c_loop_start) return c_errno = C_ERROR_NOTINLOOP;

	c_expect(';');
	c_ungetchar(';');

	fixup = c_break_fixups;
	c_break_fixups =
		(struct c_fixup *)mem_alloc(sizeof(struct c_fixup));
	c_break_fixups->next = fixup;

	if (c_pass)
		c_code_ptr->op = c_op_ba;
	c_break_fixups->pc = c_code_ptr + 1;
	c_code_ptr += 2;

	return c_errno;
}

static int c_return(char term)
{
	if (!term) return c_errno = C_ERROR_NOTINFUNC;

	if (c_pass)
		c_code_ptr->op = c_op_return;
	c_code_ptr++;

	c_expect(';');
	c_ungetchar(';');

	return c_errno;
}

static int c_block(char term, struct c_ident *vars)
{
	struct c_ident *locals = vars;
	char *token;

	while (*(token = c_gettoken()) != term) {
		if (c_errno) {
			if (!term && c_errno == C_ERROR_EOF)
				c_errno = C_ERROR_NONE;
			break;
		}
		if (*token == ' ') continue;

		if (*token == '{') {
			if (!term) return c_errno = C_ERROR_NOTINFUNC;

			if (term == ';') term = '}'; else
			if (c_block('}', locals)) break; else continue;
		} else

		if (!strcmp(token, "void") || !strcmp(token, "int")) {
			if (c_define(term, &locals, vars)) break;
		} else

		if (!strcmp(token, "if") || !strcmp(token, "while")) {
			if (c_cond(term, locals, token)) break;
		} else

		if (!strcmp(token, "else"))
			return c_errno = C_ERROR_NOTINIF;
		else

		if (!strcmp(token, "continue")) {
			if (c_continue()) break;
		} else

		if (!strcmp(token, "break")) {
			if (c_break(term, locals)) break;
		} else

		if (!strcmp(token, "return")) {
			if (c_return(term)) break;
		} else

		if (*token != ';')
			if (c_expr(term, locals, token)) break;

		if (c_errno) break;
	}

	c_free_ident(locals, vars);

	if (c_errno && c_EOF) c_errno = C_ERROR_EOF;

	return c_errno;
}

#ifdef __GNUC__
static void c_direct(union c_insn *addr);
#endif

int c_compile(int (*ext_getchar)(), void (*ext_rewind)(),
	struct c_ident *externs)
{
#ifdef __GNUC__
	c_direct(NULL);
#endif

	c_ext_getchar = ext_getchar;
	c_ext_rewind = ext_rewind;

	c_code_start = NULL;
	c_data_start = NULL;

	for (c_pass = 0; c_pass < 2; c_pass++) {
		c_init();
		c_block(0, externs);

		if (!c_pass) {
			c_free_ident(c_funcs, NULL);
			c_free_fixup(c_break_fixups, NULL);
		}

		if (c_errno || c_pass) break;

		c_code_start = mem_alloc((size_t)c_code_ptr);
		c_data_start = mem_alloc((size_t)c_data_ptr);
	}

	return c_errno;
}

struct c_ident *c_lookup(char *name)
{
	return c_find_ident(c_funcs, NULL, name);
}

void c_execute(struct c_ident *fn)
{
#ifndef __GNUC__
	register void (*op)();
#endif

	if (!fn) return;

#ifdef __GNUC__
	c_direct(fn->addr);
#else
	c_stack[0].pc = NULL;
	c_sp = &c_stack[2];

	c_pc = fn->addr;
	do {
		op = (c_pc++)->op;
		op();
	} while (c_pc);
#endif
}

#ifdef __GNUC__

static void c_direct(union c_insn *addr)
{
	union c_insn *pc = addr;
	union c_insn *sp = c_stack;
	c_int imm = 0;

	static void *ops[] = {
		&&op_index,
		&&op_assign,
		&&op_add_a,
		&&op_sub_a,
		&&op_mul_a,
		&&op_div_a,
		&&op_mod_a,
		&&op_or_a,
		&&op_xor_a,
		&&op_and_a,
		&&op_shl_a,
		&&op_shr_a,
		&&op_or_i,
		&&op_and_b,
		&&op_not_b,
		&&op_eq,
		&&op_sub,
		&&op_gt,
		&&op_lt,
		&&op_ge,
		&&op_le,
		&&op_or_i,
		&&op_xor_i,
		&&op_and_i,
		&&op_shl,
		&&op_shr,
		&&op_add,
		&&op_sub,
		&&op_mul,
		&&op_div,
		&&op_mod,
		&&op_not_i,
		&&op_neg,
		&&op_inc_l,
		&&op_dec_l,
		&&op_inc_r,
		&&op_dec_r,
	};

	if (!addr) {
		int op = 0;

		c_op_return = &&op_return;
		c_op_bz = &&op_bz;
		c_op_ba = &&op_ba;
		c_op_push_imm = &&op_push_imm;
		c_op_push_mem = &&op_push_mem;
		c_op_pop = &&op_pop;

		c_op_push_imm_imm = &&op_push_imm_imm;
		c_op_push_imm_mem = &&op_push_imm_mem;
		c_op_push_mem_imm = &&op_push_mem_imm;
		c_op_push_mem_mem = &&op_push_mem_mem;

		do {
			c_ops[op].op = ops[op];
		} while (c_ops[++op].prec);

		return;
	}

	goto *(pc++)->op;

op_return:
	return;

op_bz:
	if (imm) {
		pc += 2;
		sp -= 2;
		goto *(pc - 1)->op;
	}
	sp -= 2;

op_ba:
	pc = pc->pc;
	goto *(pc++)->op;

op_push_imm:
	(sp - 2)->imm = imm;
	imm = pc->imm;
	pc += 2;
	sp += 2;
	goto *(pc - 1)->op;

op_push_mem:
	(sp - 2)->imm = imm;
	imm = *((sp + 1)->mem = pc->mem);
	pc += 2;
	sp += 2;
	goto *(pc - 1)->op;

op_pop:
	sp -= 2;
	goto *(pc++)->op;

op_push_imm_imm:
	(sp - 2)->imm = imm;
	sp->imm = pc->imm;
	imm = (pc + 1)->imm;
	pc += 3;
	sp += 4;
	goto *(pc - 1)->op;

op_push_imm_mem:
	(sp - 2)->imm = imm;
	sp->imm = pc->imm;
	imm = *((sp + 3)->mem = (pc + 1)->mem);
	pc += 3;
	sp += 4;
	goto *(pc - 1)->op;

op_push_mem_imm:
	(sp - 2)->imm = imm;
	sp->imm = *((sp + 1)->mem = pc->mem);
	imm = (pc + 1)->imm;
	pc += 3;
	sp += 4;
	goto *(pc - 1)->op;

op_push_mem_mem:
	(sp - 2)->imm = imm;
	sp->imm = *((sp + 1)->mem = pc->mem);
	imm = *((sp + 3)->mem = (pc + 1)->mem);
	pc += 3;
	sp += 4;
	goto *(pc - 1)->op;

op_index:
	imm = *((sp - 3)->mem += imm);
	sp -= 2;
	goto *(pc++)->op;

op_assign:
	*(sp - 3)->mem = imm;
	sp -= 2;
	goto *(pc++)->op;

op_add_a:
	imm = *(sp - 3)->mem += imm;
	sp -= 2;
	goto *(pc++)->op;

op_sub_a:
	imm = *(sp - 3)->mem -= imm;
	sp -= 2;
	goto *(pc++)->op;

op_mul_a:
	imm = *(sp - 3)->mem *= imm;
	sp -= 2;
	goto *(pc++)->op;

op_div_a:
	imm = *(sp - 3)->mem /= imm;
	sp -= 2;
	goto *(pc++)->op;

op_mod_a:
	imm = *(sp - 3)->mem %= imm;
	sp -= 2;
	goto *(pc++)->op;

op_or_a:
	imm = *(sp - 3)->mem |= imm;
	sp -= 2;
	goto *(pc++)->op;

op_xor_a:
	imm = *(sp - 3)->mem ^= imm;
	sp -= 2;
	goto *(pc++)->op;

op_and_a:
	imm = *(sp - 3)->mem &= imm;
	sp -= 2;
	goto *(pc++)->op;

op_shl_a:
	imm = *(sp - 3)->mem <<= imm;
	sp -= 2;
	goto *(pc++)->op;

op_shr_a:
	imm = *(sp - 3)->mem >>= imm;
	sp -= 2;
	goto *(pc++)->op;

op_or_i:
	imm |= (sp - 4)->imm;
	sp -= 2;
	goto *(pc++)->op;

op_and_b:
	imm = (sp - 4)->imm && imm;
	sp -= 2;
	goto *(pc++)->op;

op_not_b:
	imm = !imm;
	goto *(pc++)->op;

op_eq:
	imm = (sp - 4)->imm == imm;
	sp -= 2;
	goto *(pc++)->op;

op_gt:
	imm = (sp - 4)->imm > imm;
	sp -= 2;
	goto *(pc++)->op;

op_lt:
	imm = (sp - 4)->imm < imm;
	sp -= 2;
	goto *(pc++)->op;

op_ge:
	imm = (sp - 4)->imm >= imm;
	sp -= 2;
	goto *(pc++)->op;

op_le:
	imm = (sp - 4)->imm <= imm;
	sp -= 2;
	goto *(pc++)->op;

op_xor_i:
	imm ^= (sp - 4)->imm;
	sp -= 2;
	goto *(pc++)->op;

op_and_i:
	imm &= (sp - 4)->imm;
	sp -= 2;
	goto *(pc++)->op;

op_shl:
	imm = (sp - 4)->imm << imm;
	sp -= 2;
	goto *(pc++)->op;

op_shr:
	imm = (sp - 4)->imm >> imm;
	sp -= 2;
	goto *(pc++)->op;

op_add:
	imm += (sp - 4)->imm;
	sp -= 2;
	goto *(pc++)->op;

op_sub:
	imm = (sp - 4)->imm - imm;
	sp -= 2;
	goto *(pc++)->op;

op_mul:
	imm *= (sp - 4)->imm;
	sp -= 2;
	goto *(pc++)->op;

op_div:
	imm = (sp - 4)->imm / imm;
	sp -= 2;
	goto *(pc++)->op;

op_mod:
	imm = (sp - 4)->imm % imm;
	sp -= 2;
	goto *(pc++)->op;

op_not_i:
	imm = ~imm;
	goto *(pc++)->op;

op_neg:
	imm = -imm;
	goto *(pc++)->op;

op_inc_l:
	*(sp - 1)->mem = ++imm;
	goto *(pc++)->op;

op_dec_l:
	*(sp - 1)->mem = --imm;
	goto *(pc++)->op;

op_inc_r:
	*(sp - 1)->mem = imm + 1;
	goto *(pc++)->op;

op_dec_r:
	*(sp - 1)->mem = imm - 1;
	goto *(pc++)->op;
}

#endif

static void c_f_op_return()
{
	c_pc = (c_sp -= 2)->pc;
}

static void c_f_op_bz()
{
	if ((c_sp -= 2)->imm)
		c_pc++;
	else
		c_pc = c_pc->pc;
}

static void c_f_op_ba()
{
	c_pc = c_pc->pc;
}

static void c_f_op_push_imm()
{
	c_sp->imm = (c_pc++)->imm;
	c_sp += 2;
}

static void c_f_op_push_mem()
{
	(c_sp++)->imm = *c_pc->mem;
	(c_sp++)->mem = (c_pc++)->mem;
}

static void c_f_op_pop()
{
	c_sp -= 2;
}

static void c_f_op_push_imm_imm()
{
	c_sp->imm = (c_pc++)->imm;
	(c_sp + 2)->imm = (c_pc++)->imm;
	c_sp += 4;
}

static void c_f_op_push_imm_mem()
{
	c_sp->imm = (c_pc++)->imm;
	(c_sp + 2)->imm = *c_pc->mem;
	(c_sp + 3)->mem = (c_pc++)->mem;
	c_sp += 4;
}

static void c_f_op_push_mem_imm()
{
	c_sp->imm = *c_pc->mem;
	(c_sp + 1)->mem = (c_pc++)->mem;
	(c_sp + 2)->imm = (c_pc++)->imm;
	c_sp += 4;
}

static void c_f_op_push_mem_mem()
{
	c_sp->imm = *c_pc->mem;
	(c_sp + 1)->mem = (c_pc++)->mem;
	(c_sp + 2)->imm = *c_pc->mem;
	(c_sp + 3)->mem = (c_pc++)->mem;
	c_sp += 4;
}

static void c_op_index()
{
	c_sp -= 2;
	(c_sp - 2)->imm = *((c_sp - 1)->mem += c_sp->imm);
}

static void c_op_assign()
{
	c_sp -= 2;
	(c_sp - 2)->imm = *(c_sp - 1)->mem = c_sp->imm;
}

static void c_op_add_a()
{
	c_sp -= 2;
	(c_sp - 2)->imm = *(c_sp - 1)->mem += c_sp->imm;
}

static void c_op_sub_a()
{
	c_sp -= 2;
	(c_sp - 2)->imm = *(c_sp - 1)->mem -= c_sp->imm;
}

static void c_op_mul_a()
{
	c_sp -= 2;
	(c_sp - 2)->imm = *(c_sp - 1)->mem *= c_sp->imm;
}

static void c_op_div_a()
{
	c_sp -= 2;
	(c_sp - 2)->imm = *(c_sp - 1)->mem /= c_sp->imm;
}

static void c_op_mod_a()
{
	c_sp -= 2;
	(c_sp - 2)->imm = *(c_sp - 1)->mem %= c_sp->imm;
}

static void c_op_or_a()
{
	c_sp -= 2;
	(c_sp - 2)->imm = *(c_sp - 1)->mem |= c_sp->imm;
}

static void c_op_xor_a()
{
	c_sp -= 2;
	(c_sp - 2)->imm = *(c_sp - 1)->mem ^= c_sp->imm;
}

static void c_op_and_a()
{
	c_sp -= 2;
	(c_sp - 2)->imm = *(c_sp - 1)->mem &= c_sp->imm;
}

static void c_op_shl_a()
{
	c_sp -= 2;
	(c_sp - 2)->imm = *(c_sp - 1)->mem <<= c_sp->imm;
}

static void c_op_shr_a()
{
	c_sp -= 2;
	(c_sp - 2)->imm = *(c_sp - 1)->mem >>= c_sp->imm;
}

static void c_op_or_i()
{
	c_sp -= 2;
	(c_sp - 2)->imm |= c_sp->imm;
}

static void c_op_and_b()
{
	c_sp -= 2;
	(c_sp - 2)->imm = (c_sp - 2)->imm && c_sp->imm;
}

static void c_op_not_b()
{
	(c_sp - 2)->imm = !(c_sp - 2)->imm;
}

static void c_op_eq()
{
	c_sp -= 2;
	(c_sp - 2)->imm = (c_sp - 2)->imm == c_sp->imm;
}

static void c_op_gt()
{
	c_sp -= 2;
	(c_sp - 2)->imm = (c_sp - 2)->imm > c_sp->imm;
}

static void c_op_lt()
{
	c_sp -= 2;
	(c_sp - 2)->imm = (c_sp - 2)->imm < c_sp->imm;
}

static void c_op_ge()
{
	c_sp -= 2;
	(c_sp - 2)->imm = (c_sp - 2)->imm >= c_sp->imm;
}

static void c_op_le()
{
	c_sp -= 2;
	(c_sp - 2)->imm = (c_sp - 2)->imm <= c_sp->imm;
}

static void c_op_xor_i()
{
	c_sp -= 2;
	(c_sp - 2)->imm ^= c_sp->imm;
}

static void c_op_and_i()
{
	c_sp -= 2;
	(c_sp - 2)->imm &= c_sp->imm;
}

static void c_op_shl()
{
	c_sp -= 2;
	(c_sp - 2)->imm <<= c_sp->imm;
}

static void c_op_shr()
{
	c_sp -= 2;
	(c_sp - 2)->imm >>= c_sp->imm;
}

static void c_op_add()
{
	c_sp -= 2;
	(c_sp - 2)->imm += c_sp->imm;
}

static void c_op_sub()
{
	c_sp -= 2;
	(c_sp - 2)->imm -= c_sp->imm;
}

static void c_op_mul()
{
	c_sp -= 2;
	(c_sp - 2)->imm *= c_sp->imm;
}

static void c_op_div()
{
	c_sp -= 2;
	(c_sp - 2)->imm /= c_sp->imm;
}

static void c_op_mod()
{
	c_sp -= 2;
	(c_sp - 2)->imm %= c_sp->imm;
}

static void c_op_not_i()
{
	(c_sp - 2)->imm = ~(c_sp - 2)->imm;
}

static void c_op_neg()
{
	(c_sp - 2)->imm = -(c_sp - 2)->imm;
}

static void c_op_inc_l()
{
	*(c_sp - 1)->mem = ++(c_sp - 2)->imm;
}

static void c_op_dec_l()
{
	*(c_sp - 1)->mem = --(c_sp - 2)->imm;
}

static void c_op_inc_r()
{
	*(c_sp - 1)->mem = (c_sp - 2)->imm + 1;
}

static void c_op_dec_r()
{
	*(c_sp - 1)->mem = (c_sp - 2)->imm - 1;
}

static void (*c_op_return)() = c_f_op_return;
static void (*c_op_bz)() = c_f_op_bz;
static void (*c_op_ba)() = c_f_op_ba;
static void (*c_op_push_imm)() = c_f_op_push_imm;
static void (*c_op_push_mem)() = c_f_op_push_mem;
static void (*c_op_pop)() = c_f_op_pop;

static void (*c_op_push_imm_imm)() = c_f_op_push_imm_imm;
static void (*c_op_push_imm_mem)() = c_f_op_push_imm_mem;
static void (*c_op_push_mem_imm)() = c_f_op_push_mem_imm;
static void (*c_op_push_mem_mem)() = c_f_op_push_mem_mem;

static struct c_op c_ops[] = {
	{1, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "[", c_op_index},
	{2, C_RIGHT_TO_LEFT, C_CLASS_BINARY, "=", c_op_assign},
	{2, C_RIGHT_TO_LEFT, C_CLASS_BINARY, "+=", c_op_add_a},
	{2, C_RIGHT_TO_LEFT, C_CLASS_BINARY, "-=", c_op_sub_a},
	{2, C_RIGHT_TO_LEFT, C_CLASS_BINARY, "*=", c_op_mul_a},
	{2, C_RIGHT_TO_LEFT, C_CLASS_BINARY, "/=", c_op_div_a},
	{2, C_RIGHT_TO_LEFT, C_CLASS_BINARY, "%=", c_op_mod_a},
	{2, C_RIGHT_TO_LEFT, C_CLASS_BINARY, "|=", c_op_or_a},
	{2, C_RIGHT_TO_LEFT, C_CLASS_BINARY, "^=", c_op_xor_a},
	{2, C_RIGHT_TO_LEFT, C_CLASS_BINARY, "&=", c_op_and_a},
	{2, C_RIGHT_TO_LEFT, C_CLASS_BINARY, "<<=", c_op_shl_a},
	{2, C_RIGHT_TO_LEFT, C_CLASS_BINARY, ">>=", c_op_shr_a},
	{3, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "||", c_op_or_i},
	{4, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "&&", c_op_and_b},
	{5, C_RIGHT_TO_LEFT, C_CLASS_LEFT, "!", c_op_not_b},
	{6, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "==", c_op_eq},
	{6, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "!=", c_op_sub},
	{6, C_LEFT_TO_RIGHT, C_CLASS_BINARY, ">", c_op_gt},
	{6, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "<", c_op_lt},
	{6, C_LEFT_TO_RIGHT, C_CLASS_BINARY, ">=", c_op_ge},
	{6, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "<=", c_op_le},
	{7, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "|", c_op_or_i},
	{7, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "^", c_op_xor_i},
	{8, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "&", c_op_and_i},
	{9, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "<<", c_op_shl},
	{9, C_LEFT_TO_RIGHT, C_CLASS_BINARY, ">>", c_op_shr},
	{10, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "+", c_op_add},
	{10, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "-", c_op_sub},
	{11, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "*", c_op_mul},
	{11, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "/", c_op_div},
	{11, C_LEFT_TO_RIGHT, C_CLASS_BINARY, "%", c_op_mod},
	{12, C_RIGHT_TO_LEFT, C_CLASS_LEFT, "~", c_op_not_i},
	{12, C_RIGHT_TO_LEFT, C_CLASS_LEFT, "-", c_op_neg},
	{12, C_LEFT_TO_RIGHT, C_CLASS_LEFT, "++", c_op_inc_l},
	{12, C_LEFT_TO_RIGHT, C_CLASS_LEFT, "--", c_op_dec_l},
	{12, C_LEFT_TO_RIGHT, C_CLASS_RIGHT, "++", c_op_inc_r},
	{12, C_LEFT_TO_RIGHT, C_CLASS_RIGHT, "--", c_op_dec_r},
	{0}
};
