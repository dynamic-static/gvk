
/*******************************************************************************

MIT License

Copyright (c) Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include "gvk-math/camera.hpp"

namespace gvk {
namespace math {

glm::mat4 Camera::view() const
{
    return view(transform.translation + transform.forward());
}

glm::mat4 Camera::view(const glm::vec3& lookAt) const
{
    return glm::lookAt(transform.translation, lookAt, transform.up());
}

glm::mat4 Camera::projection(bool flipY) const
{
    glm::mat4 m { };
    switch (projectionMode) {
    case ProjectionMode::Perspective: {
        m = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
    } break;
    case ProjectionMode::Orthographic: {
        auto w = fieldOfView * 0.5f;
        auto h = fieldOfView / aspectRatio * 0.5f;
        m = glm::ortho(-w, w, -h, h, nearPlane, farPlane);
    } break;
    default: {
    } break;
    }
    if (flipY) {
        m[1][1] *= -1;
    }
    return m;
}

Camera::Controller::~Controller()
{
}

const Camera* Camera::Controller::get_camera() const
{
    return mpCamera;
}

Camera* Camera::Controller::get_camera()
{
    return mpCamera;
}

void Camera::Controller::set_camera(Camera* pCamera)
{
    mpCamera = pCamera;
}

void FreeCameraController::update(const UpdateInfo& updateInfo)
{
    if (mpCamera) {
        if (moveEnabled) {
            glm::vec3 move { };
            move += updateInfo.moveUp ? mpCamera->transform.up() : glm::vec3 { };
            move += updateInfo.moveDown ? mpCamera->transform.down() : glm::vec3 { };
            move += updateInfo.moveLeft ? mpCamera->transform.left() : glm::vec3 { };
            move += updateInfo.moveRight ? mpCamera->transform.right() : glm::vec3 { };
            move += updateInfo.moveForward ? mpCamera->transform.forward() : glm::vec3 { };
            move += updateInfo.moveBackward ? mpCamera->transform.backward() : glm::vec3 { };
            // NOTE : Clamp instead of the old unconditional normalize() -- one bool
            //  held gives length 1 already (clamp is a no-op), two at once give
            //  length sqrt(2) and get clamped to 1, same result normalize() gave, but
            //  a zero vector now falls through untouched instead of needing an
            //  explicit div-by-zero guard.
            auto moveLengthSquared = glm::dot(move, move);
            move = moveLengthSquared > 1 ? move / glm::sqrt(moveLengthSquared) : move;
            mpCamera->transform.translation += move * moveSpeed * updateInfo.moveSpeedMultiplier * updateInfo.deltaTime;
            // NOTE : moveDelta applied directly, not integrated by moveSpeed/deltaTime
            //  like the digital move above -- this is a per-frame position delta (eg.
            //  a touch drag's frame-to-frame screen movement, pre-scaled by the
            //  caller to world units already), not a held analog axis. Matches
            //  lookDelta/fieldOfViewDelta's convention below, deliberately not the
            //  velocity-integrated digital move above. Caveat: this makes it
            //  frame-rate-dependent if something ever drives it continuously (eg. a
            //  held gamepad stick) rather than as a one-shot per-frame drag delta --
            //  fine for touch-drag today, worth revisiting if a held analog source
            //  needs this later.
            mpCamera->transform.translation +=
                mpCamera->transform.right() * updateInfo.moveDelta.x +
                mpCamera->transform.forward() * updateInfo.moveDelta.y +
                mpCamera->transform.up() * updateInfo.moveDelta.z;
        }
        if (lookEnabled) {
            auto look = updateInfo.lookDelta * lookSpeed;
            if (mVerticalLook + look.y > verticalLookMax) {
                look.y = verticalLookMax - mVerticalLook;
            } else if (mVerticalLook + look.y < verticalLookMin) {
                look.y = verticalLookMin - mVerticalLook;
            }
            mVerticalLook += look.y;
            auto horizontalRotation = glm::angleAxis(-look.x, glm::vec3{ 0, 1, 0 });
            auto verticalRotation = glm::angleAxis(look.y, glm::vec3{ 1, 0, 0 });
            mpCamera->transform.rotation = glm::normalize(horizontalRotation * mpCamera->transform.rotation * verticalRotation);
        }
        if (fieldOfViewEnabled) {
            mpCamera->fieldOfView -= updateInfo.fieldOfViewDelta * fieldOfViewSpeed;
            mpCamera->fieldOfView = glm::clamp(mpCamera->fieldOfView, fieldOfViewMin, fieldOfViewMax);
        }
    }
}

} // namespace math
} // namespace gvk
