#pragma once

#include "Vector.h"
#include "const.h"

#include <vector>
#include <array>

struct Collision;

struct Prediction
{
    Prediction() {}
    ~Prediction() {}

    struct PredictionInfo
    {
        PredictionInfo() {};
        ~PredictionInfo() {};

        void move(const vec_t& time);
        void findFirstCollision(void* pData, vec_t* time, int ballsCount);
        void findFirstCollision(void* pData, vec_t* time);
        bool isMovingOrSpinning();
        void calcVelocity();
        void calcVelocityPostCollision(const vec_t& angle);
        bool sub_1C29FA0(vec_t* smallestTime, Prediction::PredictionInfo* otherBallInfo);
        bool sub_1C2A2C0(vec_t* smallestTime, const Vector2D* pointA, const Vector2D* pointB);
        bool sub_1C2A594(vec_t* smallestTime, const Vector2D* pointA);
        void sub_1BF9BD8(void* pData, double* a4);

        int                   state, classification, pocketIndex;
        UINT32                index;
        Vector3D              spin;
        Vector2D              initialPos, predictedPosition, velocity;
        std::vector<Vector2D> positions;
        bool                  onTable, originalOnTable;
    };

    struct SceneData
    {
        int                 ballsCount;
        UINT32              ballList;
        Collision* collision;
        bool                validCushionShot, state;
        PredictionInfo      info[maxBallsCount];
        bool                forceCushionMode;
        std::array<bool, 6> activeCushions;
    } guiData, autoAimData;

    bool init();
    void initAutoAim();
    void onInitialization();
private:
    void     initSceneData(SceneData* data, const bool insert);
    void     predictFinalDestinations(SceneData* data, const int& ballsCount);
    void     updateState(SceneData* data, int& bestIndex, bool is9BallGame, bool nineBallGameRule);
    void     handleCollision(SceneData* data);
    void     handleBallBallCollision(Collision* collision);
    vec_t    getPowerAmount();
    void     setPowerAmount(vec_t value);
    Vector2D getSpinAmount();
};

extern Prediction* gPrediction;