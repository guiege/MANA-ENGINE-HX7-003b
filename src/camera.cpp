#include "Camera.h"

Camera::Camera(float left, float right, float bottom, float top)
	:m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
{
	size = glm::vec2(abs(right - left), abs(bottom - top));
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::RecalculateViewMatrix()
{
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, m_Position);
	transform = glm::translate(transform, glm::vec3(0.5 * size.x, 0.5 * size.y, 0.0f));
	transform = glm::rotate(transform, glm::radians(m_Rotation), glm::vec3(0, 0, 1));
	transform = glm::scale(transform, m_Scale);
	transform = glm::translate(transform, glm::vec3(-0.5 * size.x, -0.5 * size.y, 0.0f));

	m_ViewMatrix = glm::inverse(transform);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}