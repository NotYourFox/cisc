#ifndef TRYCATCH_H
#define TRYCATCH_H

#include <setjmp.h>

jmp_buf *g__ActiveBuf;

#define try jmp_buf __LocalJmpBuff;jmp_buf *__OldActiveBuf=g__ActiveBuf;bool __WasThrown=false;g__ActiveBuf=&__LocalJmpBuff;int cjmp=setjmp(__LocalJmpBuff);if(cjmp){__WasThrown=true;}else
#define try_s __OldActiveBuf=g__ActiveBuf;__WasThrown=false;g__ActiveBuf=&__LocalJmpBuff;cjmp=setjmp(__LocalJmpBuff);if(cjmp){__WasThrown=true;}else
#define catch(x) g__ActiveBuf=__OldActiveBuf;x=cjmp;if(__WasThrown)
#define throw(x) longjmp(*g__ActiveBuf,x);
#define raise(x, ...) printf(__VA_ARGS__);exit(x);

#endif