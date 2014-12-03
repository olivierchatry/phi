#include <PreCompile.h>
#include <Game/Camera/CameraFollowPlayer.h>
#include <Game/Player.h>
#include <Game/Level.h>

namespace Game
{
    void CameraFollowPlayer::initialize(Initialize& intialize)
    {
        mDeltaOnSpline = 0.f;
    }
    
    void CameraFollowPlayer::update(Update& update)
    {
        Player* playerEntity = (Player*)update.player;
        Level* level = (Level*)update.level;
        
        if (playerEntity == NULL || level == NULL)
            return;
        Player::Physic& player = playerEntity->getPhysic();
        
        mVelocitySmoothed.add(player.boosterVelocity);
        
        float fov = (glm::clamp(glm::length(mVelocitySmoothed.get()), 0.f, 4000.f) / 4000.0f) * glm::quarter_pi<float>() * 1.5f;

        float ratio;
        int width, height;
        
        glfwGetFramebufferSize(update.window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);

        update.projection = glm::perspective(glm::quarter_pi<float>() + fov, ratio, 0.1f, 10000.0f);
        
        
        glm::vec3 from = player.position - player.direction * 10.f + player.normal * 2.f;
        
        /*{
            level->findNearestDelta(from, mDeltaOnSpline, 1);
            float       radiusCamera = level->getRadius(mDeltaOnSpline);
            glm::vec3   pointCamera = level->getPosition(mDeltaOnSpline);
            glm::vec3   normalCamera = glm::normalize(from - pointCamera);
            glm::vec3   cameraCollisionPoint = normalCamera * radiusCamera * 1.2f + pointCamera;
            
            
            // if (glm::distance(pointCamera, cameraCollisionPoint) >
            //    glm::distance(pointCamera, from))
            //    from = cameraCollisionPoint;
        }*/
        glm::vec3 to = player.position;

        glm::vec3 normal = glm::cross(player.right, glm::normalize(to - from));
        
        update.view = glm::lookAt(from, to, normal);
        update.from = from;
        update.to = to;
    }

    void CameraFollowPlayer::destroy(Destroy& destroy)
    {
        
    }

    void CameraFollowPlayer::render(RenderArg& render)
    {
        
    }
    
}
