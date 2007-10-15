#include "shotgun.h"
#include "cpu.h"
#include "methctx.h"
#include "tuple.h"
#include "flags.h"

OBJECT blokenv_s_under_context(STATE, OBJECT ctx, OBJECT ctx_block, int start, OBJECT lst, OBJECT vlst, OBJECT locals) {
  OBJECT obj;
    
  obj = blokenv_allocate(state);
  blokenv_set_home(obj, ctx);
  blokenv_set_initial_ip(obj, I2N(start));
  blokenv_set_last_ip(obj, lst);
  blokenv_set_post_send(obj, vlst);
  blokenv_set_home_block(obj, ctx_block);
  blokenv_set_method(obj, FASTCTX(ctx)->method); // Calling method's CompiledMethod
  if(NIL_P(locals)) {
    blokenv_set_local_count(obj, I2N(0));
  } else {
    blokenv_set_local_count(obj, I2N(NUM_FIELDS(locals)));
  }
  blokenv_set_bonus(obj, locals);
  return obj;
}

OBJECT blokenv_create_context(STATE, OBJECT self, OBJECT sender, int sp) {
  OBJECT ctx;
  int cnt;
  struct fast_context *fc;
  
  ctx = object_memory_new_context(state->om);
  if(ctx >= state->om->context_last) {
    state->om->collect_now |= OMCollectYoung;
  }
  
  if(state->excessive_tracing) {
    printf("CTX:           block running %d\n", (int)ctx);
  }
  
  HEADER(ctx)->flags = 0;
  HEADER(ctx)->flags2 = 0;
  HEADER(ctx)->fields = FASTCTX_FIELDS;
  
  fc = FASTCTX(ctx);
  fc->sender = sender;
  fc->ip = FIXNUM_TO_INT(blokenv_get_initial_ip(self));
  fc->sp = sp;
  /* env lives here */
  fc->name = self;
  fc->self = Qnil;
  fc->method = blokenv_get_method(self);
  fc->block = Qnil;
  fc->literals = Qnil;
  fc->method_module = Qnil;
  
  cnt = FIXNUM_TO_INT(blokenv_get_local_count(self));
  if(cnt > 0) {
    fc->locals = tuple_new(state, cnt);
  } else {
    fc->locals = Qnil;
  }
  fc->flags = 0;
  
  /* If post send is nil, that means we're not allowed to return directly to
     the home context. */
  if(NIL_P(blokenv_get_post_send(self))) {
    fc->flags |= CTX_FLAG_NO_LONG_RETURN;
  }
  
  fc->type = FASTCTX_BLOCK;
  return ctx;
}
