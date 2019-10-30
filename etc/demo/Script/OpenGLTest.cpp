#include "OpenGLTest.hpp"
#include <glad/glad.h>

SCRIPT_DEFINATION(Script, OpenGLTest,2);
OpenGLTest::OpenGLTest(Entity* entity) : Script(entity) {
    transform = entity->addComponent<Transform>(30,30,0);
    sprite = entity->addComponent<Sprite>("../resource/girl1.png");
    sprite->draw = [this](auto view, auto projection){this->draw(view, projection);};
    PolygonShape shape(64,139);
    body = entity->addComponent<TRigidbody>(&shape, BodyType::Dynamic);
}

void OpenGLTest::update() {
    auto left = Input::getKeyStatus(SDL_SCANCODE_A);
    auto right = Input::getKeyStatus(SDL_SCANCODE_D);
    auto up = Input::getKeyStatus(SDL_SCANCODE_W);
    auto down = Input::getKeyStatus(SDL_SCANCODE_S);

    const float speed = 5;
    glm::vec2 velocity;
    velocity.x = (-left + right) * speed;
    velocity.y = (up - down) * speed;

    transform->moveBy(velocity.x, velocity.y, 0);
}

void OpenGLTest::draw(const glm::mat4& view, const glm::mat4 proj) {
    drawSprite(view, proj);
    drawOutline(view, proj);
}

void OpenGLTest::drawSprite(const glm::mat4& view, const glm::mat4 proj) {
    glActiveTexture(GL_TEXTURE0);
    sprite->getShader()->use();
    glBindTexture(GL_TEXTURE_2D, sprite->getMainTexture()->getId());
    glUniform1i(glGetUniformLocation(sprite->getShader()->ID, "texture0"), 0);

    glUniformMatrix4fv(glGetUniformLocation(sprite->getShader()->ID, "model"), 1, GL_FALSE, &transform->getModel()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sprite->getShader()->ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sprite->getShader()->ID, "projection"), 1, GL_FALSE, &proj[0][0]);
    glUniform1i(glGetUniformLocation(sprite->getShader()->ID, "hasTRigidbody"), 1);
    glm::vec2 pos = transform->position;
    sprite->getShader()->setVec2("point1", body->shape->leftPoint + pos);
    sprite->getShader()->setVec2("point2", body->shape->rightPoint + pos);

    glBindVertexArray(sprite->getMainTexture()->getVAO());

    glStencilFunc(GL_ALWAYS, 1, 0xff); //always pass the stencil test
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void OpenGLTest::drawOutline(const glm::mat4& view, const glm::mat4 proj) {
    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_FALSE);
    auto outlineShader = Shader::collection["outline"];
    outlineShader->use();
    outlineShader->setInt("texture0", 0);

    glBindTexture(GL_TEXTURE_2D, sprite->getMainTexture()->getId());
    glUniform1i(glGetUniformLocation(outlineShader->ID, "texture0"), 0);

    glUniformMatrix4fv(glGetUniformLocation(outlineShader->ID, "model"), 1, GL_FALSE, &transform->getModel()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(outlineShader->ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(outlineShader->ID, "projection"), 1, GL_FALSE, &proj[0][0]);
    glBindVertexArray(sprite->getMainTexture()->getVAO());

    glStencilFunc(GL_NOTEQUAL, 1, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}