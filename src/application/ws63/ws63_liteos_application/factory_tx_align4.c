/* WS63 factory FT2/Polar3/4 TX workaround, audited against controller bfedcd51.
 * The allocator's block+2 offset is kept intact. Only the unsubmitted factory
 * TXPD is redirected, after an overlap-safe move inside its owned EM block.
 * This is not an assertion about AES or a generic DMA ABI.
 */
#include <stdint.h>
#include <stddef.h>

struct factory_tx_span {
    uint32_t em_size, base, stride, index, kind, node_p, pd, sed, length;
};

static uint16_t get16(const volatile uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static void put16(volatile uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

/* Return 0 only after all bounds/identity checks; failure never changes EM.
 * EM geometry fields originate in uint16_t ROM globals / uint8_t node index.
 */
static int align_factory_payload(volatile uint8_t *em, const struct factory_tx_span *s,
    uint16_t *new_p)
{
    uint32_t block, end, p, aligned, i;
    if (!em || !s || !new_p || (s->em_size != 16384 && s->em_size != 32768) ||
        s->base > s->em_size || s->stride < 4 || s->stride > s->em_size ||
        s->index > 255 || (s->kind & 7) != 1 || s->length > 2047)
        return -1;
    block = s->base + s->index * s->stride;
    end = block + s->stride;
    if (end > s->em_size || s->node_p != block + 2 ||
        s->pd > s->em_size - 28 || s->sed > s->em_size - 196 ||
        s->pd + 28 > block || s->sed + 196 > block)
        return -1;
    p = get16(em + s->pd + 4);
    if (p != s->node_p || (get16(em + s->pd + 24) & 2047) != s->length)
        return -1;
    aligned = (p + 3u) & ~3u;
    if (aligned > end || s->length > end - aligned)
        return -1;
    /* Move backwards: source/destination overlap by length-2 bytes. No
     * headroom, neighbouring block, allocator node or descriptor length write. */
    if (aligned != p) {
        for (i = s->length; i > 0; --i)
            em[aligned + i - 1] = em[p + i - 1];
    }
    put16(em + s->pd + 4, (uint16_t)aligned);
    *new_p = (uint16_t)aligned;
    return 0;
}

#ifndef WS63_FACTORY_ALIGN4_HOST_TEST
extern uint8_t *g_evt_task_env_gle_test_mode_tx;
extern uint32_t g_em_size;
extern uint16_t g_em_gle_tx_buff_data_offset, g_em_gle_tx_buff_data_size;
extern uint16_t g_em_comm_txpd_desc_offset, g_em_comm_sed_offset;
extern int __real_evt_task_single_task_start(void *env, void *element, uint32_t urgent);

/* Bounded diagnostic record; no new AT command, owner, task or queue. */
volatile struct {
    uint32_t attempts, rejected, adjusted, submitted, submit_failed;
    uint32_t env, node, old_p, new_p, length, pd, sed;
} g_ws63_factory_tx_align4;

static int ram_span(uintptr_t p, uint32_t bytes)
{
    return p >= 0xa00000u && p <= 0xa90000u - bytes;
}

int __wrap_evt_task_single_task_start(void *environment, void *element, uint32_t urgent)
{
    volatile uint8_t *env = environment, *node, *txnode;
    volatile uint8_t *em = (volatile uint8_t *)(uintptr_t)0x49010000u;
    struct factory_tx_span s;
    uint16_t new_p;
    int result;
    /* The original caller owns the complete descriptor and buffer until this
     * call publishes the task. Other controller tasks are never modified. */
    if (environment != g_evt_task_env_gle_test_mode_tx || !environment)
        return __real_evt_task_single_task_start(environment, element, urgent);
    if (!ram_span((uintptr_t)env, 72))
        return -1;
    if (env[60] != 2 || env[63] != 2)
        return __real_evt_task_single_task_start(environment, element, urgent);
    ++g_ws63_factory_tx_align4.attempts;
    node = *(uint8_t *volatile *)(env + 68);
    txnode = *(uint8_t *volatile *)(env + 64);
    if (urgent || !ram_span((uintptr_t)node, 8) || !ram_span((uintptr_t)txnode, 6))
        goto reject;
    s.em_size = g_em_size;
    s.base = g_em_gle_tx_buff_data_offset;
    s.stride = g_em_gle_tx_buff_data_size;
    s.index = node[6]; s.kind = node[7]; s.node_p = get16(node + 4);
    s.pd = (uint32_t)g_em_comm_txpd_desc_offset + 28u * get16(txnode + 4);
    s.sed = (uint32_t)g_em_comm_sed_offset + 196u * get16(env + 42);
    s.length = get16(env + 56);
    if ((s.em_size != 16384 && s.em_size != 32768) ||
        s.pd > s.em_size || s.em_size - s.pd < 28 ||
        s.sed > s.em_size || s.em_size - s.sed < 196)
        goto reject;
    /* Bind the exact published PD and rate/coding fields to this environment. */
    if (get16(em + s.sed + 22) != s.pd ||
        (em[s.pd + 18] & 15) != ((env[61] & 3) | ((env[63] & 3) << 2)))
        goto reject;
    g_ws63_factory_tx_align4.env = (uint32_t)(uintptr_t)env;
    g_ws63_factory_tx_align4.node = (uint32_t)(uintptr_t)node;
    g_ws63_factory_tx_align4.old_p = s.node_p;
    g_ws63_factory_tx_align4.length = s.length;
    g_ws63_factory_tx_align4.pd = s.pd;
    g_ws63_factory_tx_align4.sed = s.sed;
    if (align_factory_payload(em, &s, &new_p))
        goto reject;
    g_ws63_factory_tx_align4.new_p = new_p;
    ++g_ws63_factory_tx_align4.adjusted;
    __asm__ volatile ("fence iorw, iorw" ::: "memory");
    result = __real_evt_task_single_task_start(environment, element, urgent);
    if (!result) ++g_ws63_factory_tx_align4.submitted;
    else ++g_ws63_factory_tx_align4.submit_failed;
    /* Original caller frees its unchanged nodes if publication fails. */
    return result;
reject:
    ++g_ws63_factory_tx_align4.rejected;
    return -1;
}
#endif
