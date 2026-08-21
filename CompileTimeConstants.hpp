inline int constexpr WindowInitWidth = 768, WindowInitHeight = 432;
inline float constexpr WindowInitAspectRatio = static_cast<float>(static_cast<long double>(WindowInitWidth) / static_cast<long double>(WindowInitHeight));
inline float constexpr FrustumNear = 0.1f, FrustumFar = 100.0f;
inline glm::mat4 constexpr glm_mat4_Identity(1.0f);