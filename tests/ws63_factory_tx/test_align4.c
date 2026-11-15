/* Exercise the actual production move/guards, with adjacent memory canaries. */
#define WS63_FACTORY_ALIGN4_HOST_TEST
#include "../../src/application/ws63/ws63_liteos_application/factory_tx_align4.c"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static uint8_t em[32768], old[32768], expected[2048];
static struct factory_tx_span setup(unsigned base, unsigned stride, unsigned index, unsigned len)
{
    struct factory_tx_span s = {32768, base, stride, index, 1,
        base + stride * index + 2, 256, 512, len};
    memset(em, 0xa5, sizeof(em));
    for (unsigned i = 0; i < len; ++i) expected[i] = em[s.node_p + i] = (uint8_t)(i*37+19);
    put16(em+s.pd+4, (uint16_t)s.node_p); put16(em+s.pd+24,(uint16_t)len);
    memcpy(old,em,sizeof(em));return s;
}
static void success(unsigned base, unsigned stride, unsigned index, unsigned len)
{
    struct factory_tx_span s=setup(base,stride,index,len);uint16_t p=0;
    assert(align_factory_payload(em,&s,&p)==0);
    assert((p&3)==0 && p>=s.node_p && p+len<=base+stride*(index+1));
    assert(memcmp(em+p,expected,len)==0);
    for(unsigned i=0;i<sizeof(em);++i)
        if(!((i>=p && i<p+len) || i==s.pd+4 || i==s.pd+5))assert(em[i]==old[i]);
    assert(get16(em+s.pd+24)==len);
}
static void rejected(struct factory_tx_span s)
{
    uint16_t p=0xbeef;memcpy(old,em,sizeof(em));
    assert(align_factory_payload(em,&s,&p)==-1);
    assert(p==0xbeef && memcmp(em,old,sizeof(em))==0);
}
int main(void)
{
    for(unsigned n=0;n<=1528;++n)success(0x5af4,1532,0,n);
    success(0x5af4,1532,1,255);success(0x2002,260,2,255); /* already aligned */
    struct factory_tx_span s=setup(0x5af4,1532,0,1530);rejected(s);
    s=setup(0x5af4,1532,0,255);s.kind=0;rejected(s);
    s=setup(0x5af4,1532,0,255);s.node_p+=2;rejected(s);
    s=setup(0x5af4,1532,0,255);s.pd=s.base;rejected(s);
    s=setup(0x5af4,1532,0,255);s.sed=s.base;rejected(s);
    s=setup(0x5af4,1532,0,255);s.index=255;rejected(s);
    s=setup(0x5af4,1532,0,255);s.em_size=0xffffffffu;rejected(s);
    s=setup(0x5af4,1532,0,255);put16(em+s.pd+24,254);rejected(s);
    s=setup(0x5af4,1532,0,255);put16(em+s.pd+4,(uint16_t)(s.node_p+2));rejected(s);
    puts("PASS: lengths0..1528, multiple blocks, aligned input, overlap, canaries, capacity and identity rejection");
    return 0;
}
