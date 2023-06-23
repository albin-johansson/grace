#pragma once

#ifdef GRACE_USE_SDL2

namespace grace {

class SDL final {
 public:
  [[nodiscard]] SDL();

  SDL(const SDL&) = delete;
  SDL(SDL&&) noexcept = delete;

  SDL& operator=(const SDL&) = delete;
  SDL& operator=(SDL&&) noexcept = delete;

  ~SDL() noexcept;
};

}  // namespace grace

#endif  // GRACE_USE_SDL2
