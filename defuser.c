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
    size_t count = 1;

    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t mid = (uint32_t)(challenge >> 16);
    uint32_t low = (uint32_t)challenge;

    uint32_t t1 = 3u * (high - mid) + 0x57adu;
    uint32_t t0 = (high ^ 0x370bu) + 2u * low;

    values[0] = (t1 + 0xa34eu) ^ t0;
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;
    values[0] = (uint32_t)((challenge >> 19) & 0x7f);
    for (unsigned int i = 1; i < 6; ++i) {
        uint32_t part =
            (uint32_t)((challenge >> (4 * i)) & 0x3f);
        values[i] =
            (part ^ (2u * values[i - 1])) + 7u * i;
    }

    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;

    uint32_t index = (uint32_t)(challenge & 7u);
    uint32_t x = (uint32_t)(challenge >> 32)
               ^ (uint32_t)(challenge >> 3);

    values[0] = index;

    switch (index) {
        case 0:
            values[1] = (x ^ 0x481u) + (x >> 1);
            break;
        case 1:
            values[1] = (2u * x) ^ 0x2db4u;
            break;
        case 2:
            values[1] = (x ^ 0x9849u) + (x >> 1);
            break;
        case 3:
            values[1] = 5u * x + 0x6a7u;
            break;
        case 4:
            values[1] = 5u * x + 0x1806fu;
            break;
        case 5:
            values[1] = 5u * x + 0x98deu;
            break;
        case 6:
            values[1] = (4u * x) ^ 0x7720u;
            break;
        case 7:
            values[1] = 5u * x + 0xbf9fu;
            break;
    }

    return write_answer(out, cap, values, count);
}

static uint32_t p4_walk_local(uint32_t a, uint32_t low,
                              uint32_t high, uint32_t b) {
    uint32_t mid = low + ((high - low) >> 1);

    if (a == mid) {
        return (mid + 3u) ^ b;
    }
    if (a < mid) {
        uint32_t r = p4_walk_local(a, low, mid - 1u, b);
        return (5u * r) ^ (mid + b) ^ 7u;
    } else {
        uint32_t r = p4_walk_local(a, mid + 1u, high, b);
        return (5u * r) ^ (mid + b) ^ 1u;
    }
}
int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    uint32_t a = (uint32_t)(challenge & 0x7fu);
    uint32_t b = (uint32_t)(challenge >> 7)
               ^ (uint32_t)(challenge >> 32);
    values[0] = p4_walk_local(a, 0u, 127u, b);

    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;

    static const uint32_t heads[8] = {
        9, 4, 5, 6, 11, 1, 7, 0
    };

    static const int next[12] = {
        2, 7, 10, -1, 5, 6, 11, 0, 3, 4, 8, 1
    };

    static const uint32_t node_value[12] = {
        0x065eu, 0x070au, 0x0a20u, 0x0b5fu,
        0x0444u, 0x0026u, 0x0355u, 0x0111u,
        0x0541u, 0x025eu, 0x0885u, 0x0952u
    };

    static const uint32_t node_add[12] = {
        0x863eu, 0x962du, 0x76b4u, 0x40feu,
        0xa588u, 0x098du, 0x8eedu, 0xa2f9u,
        0x9975u, 0x93d6u, 0x4a0bu, 0x2b95u
    };

    uint32_t index = (uint32_t)(challenge & 7u);
    uint32_t steps = (uint32_t)((challenge >> 3) & 3u);

    uint32_t x = (uint32_t)(challenge >> 6)
               ^ (uint32_t)(challenge >> 32)
               ^ 0x486cu;
    int node = (int)heads[index];
    for (uint32_t i = 0; i <= steps; i++) {
        x = (3u * x + node_add[node]) ^ node_value[node];
        values[1] = node_value[node];

        if (i < steps) {
            node = next[node];
        }
    }
    values[0] = x;
    return write_answer(out, cap, values, count);
}

static uint32_t p6_apply(uint32_t index, uint32_t x) {
    switch (index) {
        case 0:
            return (x ^ 0x7438u) + (x << 7);
        case 1:
        case 2:
            return (x >> 2) | (x << 30);

        case 3:
        case 4:
            return (4u * x + 0xb375u) ^ (5u * x);
        case 5:
            return (x ^ (x >> 5)) + 0x9e05u;
        case 6:
            return ((x << 7) | (x >> 25)) ^ 0x6f45u;
        case 7:
            return (x + 0x2f02u) ^ (x >> 7);
        default:
            return 0u;
    }
}
int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    uint32_t a = (uint32_t)(challenge & 7u);
    uint32_t b = (uint32_t)((challenge >> 3) & 7u);

    uint32_t x = (uint32_t)(challenge >> 6)
               ^ (uint32_t)(challenge >> 32);

    uint32_t r = p6_apply(a, x);

    r = r ^ 0x326cu;
    r = p6_apply(b, r);
    values[0] = r + 0xb9bfu;
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context,
                 char *out, size_t cap) {
    uint32_t a = (uint32_t)(context & 3u);

    uint32_t x = (uint32_t)(challenge >> 32)
               ^ (uint32_t)challenge;
    uint32_t y = (uint32_t)(context >> 32)
               ^ (uint32_t)context;
    uint32_t z = (a * 0x802eu)
               ^ (uint32_t)(context >> 16);
    uint32_t t = x ^ y;
    uint32_t mixed = ((t >> 1) ^ 0x2976u)
                   + (t ^ 0x802eu);
    uint32_t b = mixed ^ z;
    uint32_t c = b ^ (y + a + 0x2976u + (x >> 1));
    if (out == NULL || cap == 0) {
        return 1;
    }
    int n = snprintf(
        out, cap,
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOPEN %u %u %u",
        (unsigned int)a,
        (unsigned int)b,
        (unsigned int)c
    );

    if (n < 0 || (size_t)n >= cap) {
        return 1;
    }
    return 0;
}
