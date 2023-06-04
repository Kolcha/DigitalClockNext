#pragma once

// TODO: consider rename using snake case
template<typename Context>
class StateGuard {
public:
  explicit StateGuard(Context* ctx) : _ctx(ctx) { save(_ctx); }
  ~StateGuard() { restore(_ctx); }

protected:
  // no implementation, specialization must be provided
  inline void save(Context* ctx);
  inline void restore(Context* ctx);

private:
  Context* _ctx;
};
