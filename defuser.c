#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    
    uint32_t low = (uint32_t)challenge;
    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t middle = (uint32_t)(challenge >> 16);

    uint32_t step0 = (low + high * 8u) ^ 0x63acu;
    uint32_t step1 = (high * 3u) ^ (middle + 0xe1cbu);
    uint32_t step2 = (low + 0x2b86u) ^ step1;

    values[0] = step2 ^ step0;
    count = 1;
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    values[0] = (uint32_t)(
        (((challenge >> 43) * 2u) & 0x3eu) + 0x13u
    );

    uint32_t shift = 18u;

    for (uint32_t i = 1; i < 6; ++i) {
        uint32_t part =
            (uint32_t)((challenge >> shift) & 0x1fu);

        values[i] =
            (values[i - 1] * 3u) ^ (part + i);

        shift += 5u;
    }

    count = 6;
    return write_answer(out, cap, values, count);
}


int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t index = (uint32_t)challenge & 7u;

    uint32_t mixed =
        (uint32_t)(challenge >> 32)
        ^ (uint32_t)(challenge >> 3);

    uint32_t answer = 0;

    switch (index) {
        case 0:
            /* jump table: index 0 -> p3_case_5 */
            answer = mixed * 5u + 0x2ac29u;
            break;

        case 1:
            /* jump table: index 1 -> p3_case_4 */
            answer = mixed * 3u + 0xe428u;
            break;

        case 2:
            /* jump table: index 2 -> p3_case_1 */
            answer = (mixed * 2u) ^ 0xc087u;
            break;

        case 3:
            /* jump table: index 3 -> p3_case_6 */
            answer = (mixed ^ 0x2f90u) + (mixed >> 2);
            break;

        case 4:
            /* jump table: index 4 -> p3_case_3 */
            answer = mixed * 5u + 0x16df0u;
            break;

        case 5:
            /* jump table: index 5 -> p3_case_7 */
            answer = (mixed * 4u) ^ 0x7e34u;
            break;

        case 6:
            /* jump table: index 6 -> p3_case_0 */
            answer = (mixed ^ 0xd25du) + (mixed >> 1);
            break;

        case 7:
            /* jump table: index 7 -> p3_case_2 */
            answer = mixed * 3u + 0xd107u;
            break;
    }

    values[0] = index;
    values[1] = answer;
    count = 2;

    return write_answer(out, cap, values, count);
}


#include <stdint.h>

static uint32_t p4_walk(uint32_t target,
                        uint32_t low,
                        uint32_t high,
                        uint32_t key)
{
    uint32_t mid = low + (high - low) / 2u;
if (mid == target) {
    return (mid + 0x63u) ^ key;
}

uint32_t temp = mid + key;
uint32_t result;

if (target > mid) {
    result = p4_walk(target, mid + 1u, high, key);
    result = result * 3u;
    result ^= temp;
    result ^= 0x15u;
} else {
    result = p4_walk(target, low, mid - 1u, key);
    result = result * 3u;
    result ^= temp;
    result ^= 0x0fu;
}

return result;
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
uint32_t target = (uint32_t)challenge & 0x7fu;

uint32_t key =
    (uint32_t)(challenge >> 7)
    ^ (uint32_t)(challenge >> 32);

values[0] = p4_walk(target, 0u, 127u, key);
count = 1;
    // TODO: 根据 phase_04 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    return write_answer(out, cap, values, count);
}
typedef struct P5Node {
    uint32_t value;
    uint32_t data;
    const struct P5Node *next;
} P5Node;

/*
 * 节点索引：
 * 0: 0x405ba0
 * 1: 0x405bb0
 * 2: 0x405bc0
 * 3: 0x405bd0
 * 4: 0x405be0
 * 5: 0x405bf0
 * 6: 0x405c00
 * 7: 0x405c10
 * 8: 0x405c20
 * 9: 0x405c30
 * 10: 0x405c40
 * 11: 0x405c50
 */
static const P5Node p5_nodes[12] = {
    [0]  = {0x4ee1u, 0x030cu, &p5_nodes[6]},
    [1]  = {0xa3fbu, 0x047eu, &p5_nodes[3]},
    [2]  = {0xf8fcu, 0x0a78u, &p5_nodes[4]},
    [3]  = {0xf187u, 0x0154u, &p5_nodes[0]},
    [4]  = {0x7e3fu, 0x0234u, &p5_nodes[11]},
    [5]  = {0x8371u, 0x0636u, &p5_nodes[2]},
    [6]  = {0x7a0bu, 0x091au, &p5_nodes[7]},
    [7]  = {0xd130u, 0x006au, &p5_nodes[10]},
    [8]  = {0x4762u, 0x072du, &p5_nodes[5]},
    [9]  = {0x50c4u, 0x0539u, &p5_nodes[8]},
    [10] = {0xec3eu, 0x0b5bu, NULL},
    [11] = {0xf83bu, 0x080au, &p5_nodes[1]}
};

static const P5Node *const p5_start[8] = {
    &p5_nodes[9],  /* 0x405c30 */
    &p5_nodes[8],  /* 0x405c20 */
    &p5_nodes[5],  /* 0x405bf0 */
    &p5_nodes[2],  /* 0x405bc0 */
    &p5_nodes[4],  /* 0x405be0 */
    &p5_nodes[11], /* 0x405c50 */
    &p5_nodes[1],  /* 0x405bb0 */
    &p5_nodes[3]   /* 0x405bd0 */
};

static uint32_t p5_traverse(uint64_t challenge,
                            uint32_t *last_data)
{
    const P5Node *node = p5_start[challenge & 7u];

    uint32_t result =
        (uint32_t)(challenge >> 6)
        ^ (uint32_t)(challenge >> 32)
        ^ node->value
        ^ 0x1f1u;

    result += node->data;
    *last_data = node->data;

    uint32_t steps = (uint32_t)(challenge >> 3) & 3u;

    for (uint32_t i = 0; i < steps; ++i) {
        node = node->next;

        result ^= node->value;
        result += node->data;
        *last_data = node->data;
    }

    return result;
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap)
{
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    uint32_t last_data = 0;

    values[0] = p5_traverse(challenge, &last_data);
    values[1] = last_data;

    count = 2;
    return write_answer(out, cap, values, count);
}

typedef uint32_t (*P6Op)(uint32_t);

static uint32_t p6_op_0(uint32_t x)
{
    return ((x << 5) + 0xfe42u) ^ (x * 5u);
}

static uint32_t p6_op_1(uint32_t x)
{
    return (x + 0x89aau) ^ (x >> 5);
}

static uint32_t p6_op_2(uint32_t x)
{
    return (x ^ 0x4e5du) + (x << 7);
}

static uint32_t p6_op_3(uint32_t x)
{
    return (x ^ (x >> 3)) + 0x3a57u;
}

static uint32_t p6_op_4(uint32_t x)
{
    return (x >> 1) | (x << 31);
}

static uint32_t p6_op_5(uint32_t x)
{
    return ((x << 7) | (x >> 25)) ^ 0xd0d5u;
}

/* 按照 0x405e00 的真实函数指针表顺序排列 */
static const P6Op p6_ops[8] = {
    p6_op_4,
    p6_op_1,
    p6_op_0,
    p6_op_2,
    p6_op_5,
    p6_op_5,
    p6_op_3,
    p6_op_1
};

static uint32_t p6_dispatch_local(uint64_t challenge)
{
    uint32_t x = (uint32_t)(challenge >> 6)
               ^ (uint32_t)(challenge >> 32);

    uint32_t index1 = (uint32_t)challenge & 7u;
    uint32_t index2 = ((uint32_t)challenge >> 3) & 7u;

    uint32_t result = p6_ops[index1](x);
    result ^= 0x12dfu;

    result = p6_ops[index2](result);
    result += 0x2778u;

    return result;
}
int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    values[0] = p6_dispatch_local(challenge);
    count = 1;
    return write_answer(out, cap, values, count);
}


static uint32_t secret_mix_local(uint32_t x)
{
    x = (x << 3) | (x >> 29);
    x += 0xb160u;
    x ^= 0x37d1u;
    return x;
}

int solve_secret(uint64_t challenge,
                 uint64_t context,
                 char *out,
                 size_t cap)
{
    uint32_t challenge_lo = (uint32_t)challenge;
    uint32_t challenge_hi = (uint32_t)(challenge >> 32);
    uint32_t context_lo = (uint32_t)context;
    uint32_t context_hi = (uint32_t)(context >> 32);

    uint32_t a = context_lo & 3u;
    uint32_t x = challenge_hi ^ challenge_lo;
    uint32_t y = context_hi ^ context_lo;

    uint32_t r9 = (a * 0xb160u) ^ (uint32_t)(context >> 16);
    uint32_t b = secret_mix_local(x ^ y) ^ r9;
    uint32_t c = b ^ (y + a + 0x37d1u + (x >> 3));

    int n = snprintf(
        out, cap,
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOPEN "
        "%" PRIu32 " %" PRIu32 " %" PRIu32,
        a, b, c
    );

    if (n < 0 || (size_t)n >= cap)
        return 1;

    return 0;
}






