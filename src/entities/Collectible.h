#pragma once

class Collectible
{
public:
    Collectible(float x = 0.0f, float y = 0.0f, int lane = 0, int value = 25);

    void update(float dt);
    void draw(float time);
    bool overlapsPlayer(float playerX) const;
    int collect();
    bool isCollected() const { return collected; }
    int getLane() const { return lane; }
    float getX() const { return x; }
    float getY() const { return baseY; }

private:
    float x;
    float baseY;
    float animationTime = 0.0f;
    int lane;
    int value;
    bool collected = false;
};
