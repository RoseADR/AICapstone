#include "Action.h"
#include "AiComponent.h"  // Include for AI-specific logic
#include "SceneManager.h"

Action::Action(const std::string& name, Actor* actor, Actor* targetActor, SceneManager* manager)
    :sceneManager(manager), actionName(name), actor(actor), targetActor(targetActor), attackCooldown(2.0f), attackTimer(0.0f) {}

DecisionTreeNode* Action::makeDecision(float deltaTime) {
    if (actionName == "Seek Player") {
        //std::cout << "[LOG]: Action - Seek Player\n";
        auto aiComponent = actor->GetComponent<AiComponent>();
        auto enemyTC = actor->GetComponent<PhysicsComponent>();
        if (aiComponent) {
            Vec3 enemyPos = enemyTC->GetPosition();
            Vec3 playerPos = targetActor->GetComponent<PhysicsComponent>()->GetPosition();
            Vec3 enemy1Move = aiComponent->Seek(enemyPos, playerPos);
            enemyTC->SetTransform(enemyTC->GetPosition() + enemy1Move * deltaTime, enemyTC->GetQuaternion());
        }
    }
    else if (actionName == "Flee Player") {
        //std::cout << "[LOG]: Action - Flee Player\n";
        auto aiComponent = actor->GetComponent<AiComponent>();
        auto enemyTC = actor->GetComponent<PhysicsComponent>();
        if (aiComponent) {
            Vec3 enemyPos = enemyTC->GetPosition();
            Vec3 playerPos = targetActor->GetComponent<PhysicsComponent>()->GetPosition();

            // Debug 
            //std::cout << "[DEBUG]: Enemy Position: (" << enemyPos.x << ", " << enemyPos.y << ", " << enemyPos.z << ")\n";
            //std::cout << "[DEBUG]: Player Position: (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << ")\n";

            Vec3 enemy1Move = aiComponent->Flee(enemyPos, playerPos);

            // Debug fleeing movement vector
            //std::cout << "[DEBUG]: Calculated Flee Direction: (" << enemy1Move.x << ", " << enemy1Move.y << ", " << enemy1Move.z << ")\n";
            
            enemyTC->SetTransform(enemyTC->GetPosition() + enemy1Move * deltaTime, enemyTC->GetQuaternion());

            // Debug new position
            Vec3 newEnemyPos = enemyTC->GetPosition();
            //std::cout << "[DEBUG]: Updated Enemy Position: (" << newEnemyPos.x << ", " << newEnemyPos.y << ", " << newEnemyPos.z << ")\n";
        }
    }
    else if (actionName == "Attack Player") {
        Ref <AiComponent> aiComponent = actor->GetComponent<AiComponent>();
        // Check cooldown
        attackTimer += deltaTime;
        if (attackTimer >= attackCooldown) {
             aiComponent->Attack(targetActor); // Execute the attack
             attackTimer = 0.0f;
             bool cool = true;
            if (cool) {
                sceneManager->playerHealth -= 10;
                std::cout << sceneManager->playerHealth;
            }
            //std::cout << "[LOG]: Attacking Player!\n";
            attackTimer = 0.0f; // Reset the timer after the attack
        }
        else {
            //std::cout << "[LOG]: Attack on cooldown. Time remaining: " << (attackCooldown - attackTimer) << "s\n";
        }
    }


    else if (actionName == "Idle") {
        //std::cout << "[LOG]: Action - Idle\n";
        // Do nothing
        // Retrieve the PhysicsComponent of the enemy
        auto physicsComponent = actor->GetComponent<PhysicsComponent>();

        // Set the velocity to zero
        physicsComponent->SetVelocity(Vec3(0.0f, 0.0f, 0.0f));

        // Set the acceleration to zero as well
        physicsComponent->SetAccel(Vec3(0.0f, 0.0f, 0.0f));

        //std::cout << "[LOG]: Velocity and acceleration reset to zero for Idle action.\n";
    }
    else {
        //std::cerr << "[ERROR]: Unknown action: " << actionName << std::endl;
    }
    return this;
}
