#include "../module.h"

class GenericDecap : public Module {
 public:
  virtual struct snobj *Init(struct snobj *arg);
  virtual void ProcessBatch(struct pkt_batch *batch);

  static const gate_idx_t kNumIGates = 1;
  static const gate_idx_t kNumOGates = 1;

 private:
  int decap_size;
};

struct snobj *GenericDecap::Init(struct snobj *arg) {
  if (!arg) return NULL;

  if (snobj_type(arg) == TYPE_INT)
    this->decap_size = snobj_uint_get(arg);
  else if (snobj_type(arg) == TYPE_MAP && snobj_eval_exists(arg, "bytes"))
    this->decap_size = snobj_eval_uint(arg, "bytes");
  else
    return snobj_err(EINVAL, "invalid argument");

  if (this->decap_size <= 0 || this->decap_size > 1024)
    return snobj_err(EINVAL, "invalid decap size");

  return NULL;
}

void GenericDecap::ProcessBatch(struct pkt_batch *batch) {
  int cnt = batch->cnt;

  int decap_size = this->decap_size;

  for (int i = 0; i < cnt; i++) snb_adj(batch->pkts[i], decap_size);

  run_next_module(this, batch);
}

ModuleClassRegister<GenericDecap> generic_decap(
    "GenericDecap", "generic_decap",
    "remove specified bytes from the beginning of packets");
