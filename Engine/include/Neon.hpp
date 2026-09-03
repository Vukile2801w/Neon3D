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
#include "Rendering/Window.hpp"
#include "Rendering/Light.hpp"

// Scene
#include "Scene/GameObject.hpp"
#include "Scene/GameObjectFactory.hpp"
#include "Scene/SceneSerializer.hpp"
#include "Scene/Behavior.hpp"
#include "Scene/Scene.hpp"

// Core
#include "Application.hpp"
#include "Transform.hpp"
#include "Logging.hpp"
#include "Assert.hpp"
#include "Camera.hpp"
#include "Input.hpp"
#include "Time.hpp"
#include "Ref.hpp"

// Assets
#include "Assets/AssetManager.hpp"

#endif