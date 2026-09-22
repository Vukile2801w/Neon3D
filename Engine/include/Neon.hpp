#ifndef NEON_HPP
#define NEON_HPP

// Events
#include "Events/MouseButtonReleasedEvent.hpp"
#include "Events/MouseButtonPressedEvent.hpp"
#include "Events/WindowLostFocusEvent.hpp"
#include "Events/MouseScrolledEvent.hpp"
#include "Events/WindowFocusedEvent.hpp"
#include "Events/WindowResizedEvent.hpp"
#include "Events/WindowClosedEvent.hpp"
#include "Events/KeyReleasedEvent.hpp"
#include "Events/KeyPressedEvent.hpp"
#include "Events/MouseMovedEvent.hpp"
#include "Events/EventBus.hpp"

// Mesh
#include "Mesh/Mesh.hpp"

// Rendering
#include "Rendering/ShaderStage.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Skybox.hpp"
#include "Rendering/Window.hpp"
#include "Rendering/Light.hpp"
#include "Rendering/PointLight.hpp"
#include "Rendering/SpotLight.hpp"
#include "Rendering/DirectionalLight.hpp"

// Scene
#include "Scene/GameObject.hpp"
#include "Scene/GameObjectFactory.hpp"
#include "Scene/LightFactory.hpp"
#include "Scene/SceneSerializer.hpp"
#include "Scene/Behavior.hpp"
#include "Scene/Scene.hpp"

// Debug
#include "Debug/Logging.hpp"

// Core
#include "Application.hpp"
#include "Transform.hpp"
#include "Assert.hpp"
#include "Camera.hpp"
#include "Input.hpp"
#include "Time.hpp"
#include "Ref.hpp"

// Assets
#include "Assets/AssetManager.hpp"

#endif