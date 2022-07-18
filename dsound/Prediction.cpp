#include "Prediction.h"
#include "GlobalVars.h"
#include "Offsets.h"
#include "Memory.h"

#include "Ball.h"
#include "Balls.h"
#include "TableInfo.h"
#include "GenericData.h"
#include "GameManager.h"
#include "UserInfo.h"
#include "Utils.h"
#include "GUI.h"
#include <string>
#include "MenuManager.h"

static Prediction prediction;
Prediction* gPrediction = &prediction;

// Forward declarations of helper functions
vec_t scalePower(const vec_t& power);
vec_t calcAngle(const Vector2D& delta);
vec_t calcAngle(const Vector2D& source, const Vector2D& Destination);

static struct Collision
{
    Collision()
    {
        ZeroMemory(this, sizeof(*this));
    }

    enum class Type : SIZE_T {
        Ball,
        Line,
        Point
    };

    bool                        valid;
    Type                        type;
    vec_t                       angle;
    Vector2D                    point;
    std::pair<UINT16, UINT16>   collisionIndex;
    Prediction::PredictionInfo* infoA, * infoB, * firstHitBall;

} collisionA /* used for GUI prediction */, collisionB /* used for Aimbot prediction */;

constexpr DWORD64 dword_35B7720 = 0x7FEFFFFFFFFFFFFF;
constexpr vec_t   dword_35B7708 = 2.0;
constexpr vec_t   dword_35B76F8 = vecZero;
constexpr vec_t   unk_35B7A20 = 1E-11;
constexpr vec_t   unk_35B3F70 = 1.5;
constexpr vec_t   dword_35B3CE8 = 0.5;
constexpr vec_t   unk_35B3F78 = 120.0;
constexpr vec_t   dword_35B3CD0 = vecZero;
constexpr vec_t   dword_35B7700 = 1.0;
constexpr vec_t   dword_35B3CD8 = 1.0;
constexpr vec_t   unk_35B3F80 = 1E-11;
constexpr vec_t   unk_35B3F88 = 2.5;
constexpr vec_t   unk_35B7A28 = 2.5;
constexpr vec_t   dword_35B7988 = 0.54;
constexpr vec_t   dword_35B7978 = 0.804;

void Prediction::PredictionInfo::calcVelocityPostCollision(const vec_t& angle)
{
    vec_t    v13 = cos(angle);
    vec_t    v14 = sin(angle);
    Vector2D ballVelocity = this->velocity;
    Vector3D ballSpin = this->spin;

    constexpr vec_t ballRadius = Ball::getRadius();

    vec_t v18 = v13 * ballVelocity.x - v14 * ballVelocity.y;
    vec_t v19 = v18 - ballRadius * ballSpin.z;
    vec_t v20 = v14 * ballVelocity.x + v13 * ballVelocity.y;
    vec_t v21 = v18 - ballRadius * ballSpin.z;
    if (v19 < dword_35B76F8)
        v21 = -v19;

    vec_t v22 = v21 / unk_35B7A28;
    vec_t v23 = v20;
    if (v20 < dword_35B76F8)
        v23 = -v20;

    vec_t v24 = 0.2 * dword_35B7708 * v23;
    vec_t v25 = -dword_35B7700;
    if (v19 > dword_35B76F8)
        v25 = dword_35B7700;

    if (v22 < v24)
        v24 = v22;

    vec_t v26 = v25 * v24;
    vec_t v27 = ballSpin.z + unk_35B7A28 * (v25 * v24) / ballRadius;
    vec_t v28 = dword_35B7988;
    vec_t v29 = v18 - v26 / unk_35B7A28;
    vec_t v30 = -(v20 * dword_35B7978);
    this->velocity.x = v14 * v30 + v13 * v29;
    this->velocity.y = v13 * v30 - v29 * v14;
    vec_t v32 = v14 * ballSpin.x + v13 * ballSpin.y;
    vec_t v33 = v13 * ballSpin.x - v14 * ballSpin.y - v20 * v28 / ballRadius;
    this->spin.x = v14 * v32 + v13 * v33;
    this->spin.y = v13 * v32 - v33 * v14;
    this->spin.z = v27;
}

void Prediction::handleBallBallCollision(Collision* collision)
{
    Vector2D        posA, posB, velA, velB;
    PredictionInfo* ballA, * ballB;

    ballA = collision->infoA;
    ballB = collision->infoB;

    posA = ballA->predictedPosition;
    posB = ballB->predictedPosition;
    velA = ballA->velocity;
    velB = ballB->velocity;

    vec_t v8 = posA.y - posB.y;
    vec_t v9 = posA.x - posB.x;
    vec_t v10 = dword_35B7700 / sqrt(v9 * v9 + v8 * v8);
    vec_t v11 = v8 * v10;
    vec_t v12 = v9 * v10;
    vec_t v15 = -(v12 * velA.x) - velA.y * v11;
    vec_t v16 = v12 * velB.x + v11 * velB.y;
    vec_t v17 = -(v12 * v15);
    vec_t v18 = -(v11 * v15);
    ballA->velocity.x = v12 * v16 - (v17 - velA.x);
    ballA->velocity.y = v11 * v16 - (v18 - velA.y);
    ballB->velocity.x = v17 - (v12 * v16 - velB.x);
    ballB->velocity.y = v18 - (v11 * v16 - velB.y);
}

void Prediction::handleCollision(SceneData* data)
{
    Vector2D delta;
    bool     insert = (data == &this->guiData);

    if (insert)
        data->collision->infoA->positions.push_back(data->collision->infoA->predictedPosition);

    switch (data->collision->type)
    {
    case Collision::Type::Ball:
        handleBallBallCollision(data->collision);
        if (insert)
            data->collision->infoB->positions.push_back(data->collision->infoB->predictedPosition);
        if (data->collision->firstHitBall == nullptr)
            data->collision->firstHitBall = data->collision->infoB;

        break;
    case Collision::Type::Line:
        data->collision->infoA->calcVelocityPostCollision(data->collision->angle);
        break;
    default:
        delta.x = data->collision->point.y - data->collision->infoA->predictedPosition.y;
        delta.y = -(data->collision->point.x - data->collision->infoA->predictedPosition.x);
        data->collision->angle = -calcAngle(delta);
        data->collision->infoA->calcVelocityPostCollision(data->collision->angle);
        break;
    }
}

void Prediction::PredictionInfo::calcVelocity()
{
    Vector2D ballVelocity = this->velocity;
    Vector3D ballSpin = this->spin;

    constexpr vec_t ballRadius = Ball::getRadius();

    if (this->isMovingOrSpinning()) {
        vec_t v15 = ballRadius * ballSpin.x - ballVelocity.y;
        vec_t v16 = -ballVelocity.x - ballSpin.y * ballRadius;
        vec_t v17 = sqrt(v16 * v16 + v15 * v15);
        vec_t v18 = v17 * 0.00145772594752187;
        vec_t v19 = gTimePerTick;
        if (v18 > unk_35B3F80) {
            vec_t v20 = gTimePerTick;
            if (v18 < v19)
                v20 = v17 * 0.00145772594752187;

            vec_t v21 = 196.0 * v20 / v17;
            vec_t v22 = v16 * v21;
            vec_t v23 = v15 * v21;
            ballVelocity.x += v22;
            ballVelocity.y += v23;
            ballSpin.x -= v23 * unk_35B3F88 / ballRadius;
            ballSpin.y += v22 * unk_35B3F88 / ballRadius;
        }

        if (v18 < v19) {
            vec_t v24 = ballVelocity.x;
            vec_t v25 = ballVelocity.y;
            vec_t v26 = dword_35B3CD0;
            vec_t v27 = (v19 - v18) * 10.878;
            vec_t v28 = dword_35B3CD8 - v27 / sqrt(v25 * v25 + v24 * v24);
            if (v26 > v28)
                v28 = v26;

            ballVelocity.x = v24 * v28;
            ballVelocity.y = v25 * v28;
            ballSpin.x = v25 * v28 / ballRadius;
            ballSpin.y = -(v24 * v28) / ballRadius;
        }

        vec_t v29 = 9.8 * v19;
        if (ballSpin.z <= dword_35B3CD0) {
            ballSpin.z = v29 + ballSpin.z;
            if (dword_35B3CD0 < ballSpin.z)
                ballSpin.z = dword_35B3CD0;
        }
        else
        {
            ballSpin.z = ballSpin.z - v29;
            if (dword_35B3CD0 > ballSpin.z)
                ballSpin.z = dword_35B3CD0;
        }

        this->velocity = ballVelocity;
        this->spin = ballSpin;
    }
}

void getTableBounds(Vector4D* tableBounds)
{
    tableBounds->x = -127.0;
    tableBounds->y = -63.5;
    tableBounds->z = 254.0;
    tableBounds->w = 127.0;
}

int sub_1B53EFC(vec_t* a1, vec_t* a2, vec_t* a3, vec_t* a4, Vector4D* tableBounds, vec_t* a6)
{
    double v6; // d3
    double v7; // d0
    signed int result; // r0
    double v9; // d1

    v6 = *a1;
    v7 = *a6;
    result = 1;
    if (v6 >= tableBounds->x + *a6 && *a3 <= tableBounds->x + tableBounds->z - v7) {
        v9 = tableBounds->y;
        if (*a2 >= v7 + v9) {
            result = 0;
            if (*a4 > v9 + tableBounds->w - v7)
                result = 1;
        }
    }

    return result;
}

int sub_1BF9ADC(Prediction::PredictionInfo* ballInfo, vec_t* smallestTime, Vector4D* tableBounds)
{
    double v5; // d0
    double v6; // d1
    double v7; // d4
    double v8; // d5
    double v9; // d3
    double v10; // d6
    double v11; // d2
    double v12; // d7
    double v14; // [sp+8h] [bp-50h]
    double v19; // [sp+30h] [bp-28h]
    double v20; // [sp+38h] [bp-20h]
    double v21; // [sp+40h] [bp-18h]
    double v22; // [sp+48h] [bp-10h]

    v5 = ballInfo->velocity.x;
    v6 = ballInfo->velocity.y;
    v7 = ballInfo->predictedPosition.x;
    v8 = ballInfo->predictedPosition.y;
    v9 = ballInfo->predictedPosition.x + ballInfo->velocity.x * *smallestTime;
    v10 = ballInfo->predictedPosition.x + ballInfo->velocity.x * *smallestTime;
    if (ballInfo->velocity.x > 0.0)
        v10 = ballInfo->predictedPosition.x;

    v11 = v8 + v6 * *smallestTime;
    v12 = v8 + v6 * *smallestTime;
    v22 = v10;
    if (v6 > 0.0)
        v12 = v8;

    v21 = v12;
    if (v5 > 0.0)
        v7 = v9;

    v20 = v7;
    if (v6 > 0.0)
        v8 = v11;

    v19 = v8;
    v14 = Ball::getRadius();

    return sub_1B53EFC(&v22, &v21, &v20, &v19, tableBounds, &v14);
}

bool Prediction::PredictionInfo::sub_1C29FA0(vec_t* smallestTime, Prediction::PredictionInfo* otherBallInfo)
{
    Vector2D posA = this->predictedPosition;
    Vector2D posB = otherBallInfo->predictedPosition;
    Vector2D velocityA = this->velocity;
    Vector2D velocityB = otherBallInfo->velocity;

    constexpr vec_t v14 = Ball::getRadius() + Ball::getRadius();

    vec_t v21 = posB.y - posA.y;
    vec_t v22 = posB.x - posA.x;

    vec_t v24 = dword_35B7708 * (v22 * (velocityB.x - velocityA.x) + v21 * (velocityB.y - velocityA.y));
    if (v24 >= dword_35B76F8)
        return false;

    vec_t v25 = v22 * v22;
    vec_t v26 = (velocityB.x - velocityA.x) * (velocityB.x - velocityA.x) + (velocityB.y - velocityA.y) * (velocityB.y - velocityA.y);
    vec_t v27 = (v25 + v21 * v21 - v14 * v14) * (v26 * 4.0);
    vec_t square = v24 * v24;
    if (square < v27)
        return false;

    vec_t v28 = (-v24 - sqrt(v24 * v24 - v27)) / (v26 * dword_35B7708);
    if (v28 < dword_35B76F8)
        return false;

    if (v28 - unk_35B7A20 > *smallestTime)
        return false;

    *smallestTime = v28;

    return true;
}

bool Prediction::PredictionInfo::sub_1C2A2C0(vec_t* smallestTime, const Vector2D* pointA, const Vector2D* pointB)
{
    Vector2D ballPos = this->predictedPosition;
    Vector2D ballVelocity = this->velocity;
    bool     isMoving = ballVelocity.isNull() == false;

    if (isMoving) {
        vec_t v17 = pointB->y - pointA->y;
        vec_t v18 = pointB->x - pointA->x;
        vec_t v19 = sqrt(v18 * v18 + v17 * v17);
        vec_t v23 = dword_35B7700;
        vec_t v7 = v17 * ballVelocity.x;
        vec_t v9 = v17 * ballVelocity.x - v18 * ballVelocity.y;
        vec_t v6 = dword_35B76F8;
        if (v9 != dword_35B76F8) {
            vec_t v8 = -(v18 * (v23 / v19));
            vec_t v7 = v17 * (v23 / v19);
            vec_t v27 = ballPos.y - pointA->y - Ball::getRadius() * v8;
            vec_t v28 = ballPos.x - pointA->x - v7 * Ball::getRadius();
            vec_t v29 = (v27 * ballVelocity.x - v28 * ballVelocity.y) / v9;
            if (v29 > dword_35B76F8 && v29 < dword_35B7700) {
                v9 = (v18 * v27 - v17 * v28) / v9;
                if (v9 > dword_35B76F8 && v9 - unk_35B7A20 <= *smallestTime) {
                    ballVelocity.y *= v8;
                    ballVelocity.x = v7 * ballVelocity.x + ballVelocity.y;
                    if (ballVelocity.x <= v6) {
                        *smallestTime = v9;
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool Prediction::PredictionInfo::sub_1C2A594(vec_t* smallestTime, const Vector2D* pointA)
{
    vec_t    squareA, squareB, radiusSquare;
    Vector2D delta;
    Vector2D ballPos = this->predictedPosition;
    Vector2D ballVelocity = this->velocity;

    delta.x = pointA->y - ballPos.y;
    delta.y = pointA->x - ballPos.x;
    vec_t v16 = -(ballVelocity.x * dword_35B7708 * delta.y) - dword_35B7708 * ballVelocity.y * delta.x;
    if (v16 >= dword_35B76F8)
        return false;

    squareA = ballVelocity.x * ballVelocity.x + ballVelocity.y * ballVelocity.y;
    squareB = delta.y * delta.y + delta.x * delta.x;
    radiusSquare = Ball::getRadius() * Ball::getRadius();
    if (squareB - v16 * v16 / (squareA * 4.0) >= radiusSquare)
        return false;

    vec_t v22 = (-v16 - sqrt(v16 * v16 - squareA * 4.0 * (squareB - radiusSquare))) / (squareA * dword_35B7708);
    if (v22 < dword_35B76F8)
        return false;

    if (v22 - unk_35B7A20 > *smallestTime)
        return false;

    *smallestTime = v22;

    return true;
}

bool sub_1BC216C(double* pTime_1, Prediction::PredictionInfo* info, const Vector2D& a3, const Vector2D& a4)
{
    double* pTime; // r8
    Prediction::PredictionInfo* v6; // r6
    double v10; // d11
    double v11; // d8
    double v12; // d12
    double v13; // d13
    double v14; // d9
    double v15; // d10
    double v16; // r0
    double v17; // d3
    double v19; // d6
    double v20; // d1
    double v21; // d5
    double v22; // d6
    double v23; // d5
    double v24; // d7
    double time; // d3


    if (info->velocity.isNull())
        goto LABEL_11;

    pTime = pTime_1;
    v6 = info;
    v10 = a4.y - a3.y;
    v11 = a4.x - a3.x;
    v12 = v6->predictedPosition.x;
    v13 = v6->predictedPosition.y;
    v14 = v6->velocity.x;
    v15 = v6->velocity.y;
    v16 = sqrt(v11 * v11 + v10 * v10);
    v17 = v10 * v14 - v11 * v15;
    if (v17 == 0.0) {
    LABEL_11:
        return false;
    }
    v19 = Ball::getRadius();
    v20 = 1.0 / v16;
    v21 = v19 * (v11 * (1.0 / v16));
    v22 = v12 - a3.x - v10 * (1.0 / v16) * v19;
    v23 = v13 - a3.y + v21;
    v24 = (v22 * -v15 - v23 * -v14) / v17;
    if (v24 <= 0.0 || v24 >= 1.0)
    {
        goto LABEL_11;
    }
    time = (v11 * v23 - v10 * v22) / v17;
    if (time <= 0.0 || (time - 1E-11 > *pTime_1))
    {
        goto LABEL_11;
    }
    if (v14 * (v10 * v20) + v15 * -(v11 * v20) > 0.0)
    {
        goto LABEL_11;
    }

    *pTime = time;
    return true;
}

void Prediction::PredictionInfo::sub_1BF9BD8(void* pData, vec_t* smallestTime)
{
    int             index, nextIndex, count;
    bool            result;
    vec_t           angle;
    const Vector2D* point, * nextPoint;

    Prediction::SceneData* data = reinterpret_cast<Prediction::SceneData*>(pData);
    const auto& tableShape = TableInfo::getTableShape();

    index = 0;
    count = tableShape.size();
    if (index < count) {
        do {
            nextIndex = index + 1;
            point = &tableShape.at(index);
            nextPoint = &tableShape.at(nextIndex % count);
            result = sub_1BC216C(smallestTime, this, *point, *nextPoint); //this->sub_1C2A2C0(smallestTime, point, nextPoint);
            if (result) {
                angle = calcAngle(*nextPoint, *point);
                data->collision->valid = true;
                data->collision->infoA = this;
                data->collision->type = Collision::Type::Line;
                data->collision->angle = -angle;
            }

            result = this->sub_1C2A594(smallestTime, point);
            if (result) {
                data->collision->valid = true;
                data->collision->infoA = this;
                data->collision->point = *point;
                data->collision->type = Collision::Type::Point;
            }

        } while (++index < count);
    }
}

void Prediction::PredictionInfo::findFirstCollision(void* pData, vec_t* time)
{
    int             index, state;
    bool            result;
    vec_t* smallestTime, radiusSquare, temp;
    Vector2D        delta, tempVelocity;
    Vector4D        tableBounds;
    PredictionInfo* ballInfo;
    SceneData* data = reinterpret_cast<SceneData*>(pData);

    constexpr vec_t pocketRadius = TableInfo::getPocketRadius();
    const auto* pocketsPositions = &TableInfo::getPocketsPositions();

    state = this->state;
    smallestTime = time;
    if (state == 1) {
        index = (this->classification == 8) ? 1 : this->index + 1;
        if (index < data->ballsCount) {
            do {
                ballInfo = &data->info[index];
                if (ballInfo->state == 1) {
                    result = this->sub_1C29FA0(smallestTime, ballInfo);
                    if (result) {
                        data->collision->valid = true;
                        data->collision->infoA = this;
                        data->collision->type = Collision::Type::Ball;
                        data->collision->infoB = ballInfo;
                    }
                }

            } while (++index < data->ballsCount);
        }
    }

    getTableBounds(&tableBounds);
    if (sub_1BF9ADC(this, smallestTime, &tableBounds) != 1) {
        goto Label;
    }

    tempVelocity = this->velocity;
    if (state == 2) {
        this->velocity.x += (tableBounds.x + tableBounds.z * dword_35B3CE8 - this->predictedPosition.x) * (*smallestTime * unk_35B3F70);
        this->velocity.y += (tableBounds.y + dword_35B3CE8 * tableBounds.w - this->predictedPosition.y) * (*smallestTime * unk_35B3F70);
    }
    else if (state == 1 && this->classification != 8) {
        index = 0;
        for (const auto& pocketPosition : *pocketsPositions) {
            delta.x = pocketPosition.x - this->predictedPosition.x;
            delta.y = pocketPosition.y - this->predictedPosition.y;
            temp = delta.x * delta.x + delta.y * delta.y;
            radiusSquare = pocketRadius * pocketRadius;
            if (temp < radiusSquare) {
                tempVelocity.x += delta.x * (*smallestTime * unk_35B3F78);
                tempVelocity.y += delta.y * (*smallestTime * unk_35B3F78);
                this->velocity = tempVelocity;
                radiusSquare = Ball::getRadius() * Ball::getRadius();
                if (temp < radiusSquare) {
                    this->state = 2;
                    this->pocketIndex = index;
                }
            }

            index++;
        }
    }

    this->sub_1BF9BD8(pData, smallestTime);
Label:
    if (state == 2) {
        this->state = 3;
        this->onTable = false;
        this->velocity.nullify();
        this->spin.nullify();
    }
}

void Prediction::PredictionInfo::move(const vec_t& time)
{
    if (this->velocity.x != vecZero) {
        this->predictedPosition.x += this->velocity.x * time;
        this->predictedPosition.y += this->velocity.y * time;
    }
}

void sub_1C2B0D8(double* a2)
{
    double* v2; // r4
    double* v3; // r5
    double v4; // d2
    unsigned int v5; // r0
    double v6; // d1
    double v7; // d0
    double v9; // [sp+8h] [bp-120h]
    char s[256]; // [sp+14h] [bp-114h]
    int v11; // [sp+114h] [bp-14h]

    v2 = a2;
    v3 = a2;
    v4 = *a2;
    if (*a2 < *(double*)&dword_35B76F8)
        v4 = -*a2;
    if (v4 >= 10000.0)
    {
        v7 = floor(*a2);
    }
    else
    {
        snprintf(s, 0xFFu, "%lf", *v3);
        v5 = strlen(s);
        v6 = 6.0;
        if (*v3 < *(double*)&dword_35B76F8)
            v6 = 6.0 + *(double*)&dword_35B7700;
        if (v5 > (unsigned int)v6)
            s[(unsigned int)v6] = 0;
        sscanf(s, "%lf", &v9);
        v7 = v9;
    }

    *v2 = v7;
}

bool Prediction::init()
{
    bool            is9BallGame;
    int             i, ballsCount, dummy;
    vec_t           angle, angleCos, angleSin, powerAmount, v29;
    Vector2D        spinAmount;
    UINT32          visualGuide;
    PredictionInfo* predictionInfo;
    Prediction::SceneData* data = &this->guiData;
    PredictionInfo* cueBallInfo = &data->info[0];

    if (VisualGuide::initVisualGuide(gGlobalVars, &visualGuide)) {
        Balls::initBallList(gGlobalVars, data->ballList, ballsCount);
        data->ballsCount = ballsCount;
        is9BallGame = GameManager::is9BallGame(gGlobalVars);
        spinAmount = getSpinAmount();
        powerAmount = getPowerAmount();
        sub_1C2B0D8(&powerAmount);
        powerAmount = scalePower(powerAmount);
        angle = VisualGuide::getAimAngle(gGlobalVars, visualGuide);
        sub_1C2B0D8(&angle);
        for (i = 0UL; i < ARRAYSIZE(data->info); i++) {
            predictionInfo = &data->info[i];
            if (predictionInfo->positions.size() > 0UL)
                predictionInfo->positions.clear();

            if (i < ballsCount)
                predictionInfo->positions.reserve(20);
        }

        this->initSceneData(data, true);
        vec_t v26 = sin(angle);
        vec_t v27 = cos(angle);
        cueBallInfo->velocity.x = powerAmount * v27;
        cueBallInfo->velocity.y = powerAmount * v26;
        vec_t v31 = -(spinAmount.y * powerAmount) / Ball::getRadius();
        vec_t v45 = -(v26 * v31);
        vec_t v46 = v27 * v31;
        vec_t v47 = spinAmount.x * powerAmount / Ball::getRadius();
        cueBallInfo->spin.x = v45;
        cueBallInfo->spin.y = v46;
        cueBallInfo->spin.z = v47;

        data->collision->firstHitBall = nullptr;
        this->predictFinalDestinations(data, ballsCount);
        if (gGlobalVars->features.esp.shotState)
            this->updateState(data, dummy, is9BallGame, 0);

        for (i = 0UL; i < ballsCount; i++) {
            predictionInfo = &data->info[i];
            predictionInfo->positions.push_back(predictionInfo->predictedPosition);
        }

        return true;
    }

    return false;
}

static bool canNominatePocket(int myClassification, bool is9BallGame, Prediction::SceneData* data, bool isInitialized)
{
    UINT32 blackBall;
    int    classification;

    if (myClassification != -1) {
        if (GameManager::getPocketNominationMode(gGlobalVars) == 1UL)
            return true;

        if (is9BallGame) {
            return false;
        }
        else
        {
            if (isInitialized == false) {
                blackBall = Balls::getBall(gGlobalVars, 8UL, data->ballList);
                classification = Ball::classification(gGlobalVars, blackBall);
            }
            else
            {
                classification = data->info[8].classification;
            }

            if (GameManager::getPocketNominationMode(gGlobalVars) == 0UL && myClassification == classification) {
                return true;
            }
        }
    }

    return false;
}

static void getPocketPositionByIndex(const int& pocketIndex, int* pos)
{
    Vector2D    pocketPos;
    const auto& pockestPositions = TableInfo::getPocketsPositions();

    pocketPos = pockestPositions.at(pocketIndex);
    pocketPos = pocketPos.toScreen();

    pos[0] = static_cast<int>(pocketPos.x);
    pos[1] = static_cast<int>(pocketPos.y);
}

static bool finishedPlayDelay(bool isPlayerTurn)
{
    bool           result;
    float          delay;
    static DWORD64 playerTimer = 0ULL;

    if (isPlayerTurn == false) {
        playerTimer = 0ULL;
        return false;
    }
    else
    {
        if (playerTimer == 0ULL)
            playerTimer = GetTickCount64();
    }

    static bool  hasSavedDelay = false;
    static float savedDelay;

    if (gGlobalVars->features.autoPlayDelayMode == 0) {
        delay = gGlobalVars->features.autoPlayDelay;
    }
    else
    {
        if (hasSavedDelay == false) {
            savedDelay = getRandomFloat(gGlobalVars->features.autoPlayDelayBounds[0], gGlobalVars->features.autoPlayDelayBounds[1]);
            hasSavedDelay = true;
        }

        delay = savedDelay;
    }

    result = (GetTickCount64() - playerTimer) > static_cast<int>(delay / 0.001f);
    if (result)
        hasSavedDelay = false;

    return result;
}

void normalizeAngle(vec_t& angle)
{
    vec_t newAngle;

    newAngle = angle;
    if (newAngle >= maxAngle) {
        newAngle = fmod(newAngle, maxAngle);
    }
    else
    {
        if (newAngle < vecZero) {
            newAngle = maxAngle - fmod(-newAngle, maxAngle);
        }
    }

    angle = newAngle;
}

void Prediction::initAutoAim()
{
    int             index, index2, count, clickPos[2], bestIndex, myClassification, nominatedPocket;
    bool            nineBallGameRule, is9BallGame, isGoldenShot, isPlayerTurn, canNominate, * activeCushion;
    vec_t           angle, angleDelta, angleDelta2, actualAngle, angleCos, angleSin, power, scaledPower, v29, v30;
    vec_t           powerAddition, angleAddition;
    Ball* blackBall;
    Vector2D        spinAmount, velocity, nullVector, screenCenter;
    UINT32          visualGuide;
    SceneData* data;
    PredictionInfo* cueBallInfo, * predictionInfo;

    if (MenuManager::isInGame(gGlobalVars) == false)
        return;

    isPlayerTurn = GameManager::isPlayerTurn();
    if (finishedPlayDelay(isPlayerTurn) == false)
        return;

    if (isPlayerTurn == false)
        return;

    if (GameManager::isAnyPopupOpen(gGlobalVars))
        return;

    isGoldenShot = (GameManager::getGameMode(gGlobalVars) == 16);
    if (isGoldenShot)
        return;

    if (VisualGuide::initVisualGuide(gGlobalVars, &visualGuide) == false)
        return;

    data = &this->autoAimData;
    cueBallInfo = &data->info[0];

    is9BallGame = GameManager::is9BallGame(gGlobalVars);
    nineBallGameRule = is9BallGame;

    Balls::initBallList(gGlobalVars, data->ballList, data->ballsCount);
    count = data->ballsCount;
    spinAmount = this->getSpinAmount();
    actualAngle = VisualGuide::getAimAngle(gGlobalVars, visualGuide);

    myClassification = GameManager::playerClassification(gGlobalVars, 0);
    if (gGlobalVars->features.automatic == 2) {
        canNominate = canNominatePocket(myClassification, is9BallGame, data, false);
        if (canNominate) {
            nominatedPocket = GameManager::getNominatedPocket(gGlobalVars);
            if (nominatedPocket > 6)
                return;
        }
        if (gGlobalVars->features.humanizedPower) {
            power = 0.1;
        }
        else
        {
            power = gGlobalVars->features.autoPlayMaxPower * 0.01f;
        }
    }
    else
    {
        power = this->getPowerAmount();
    }

    if (gGlobalVars->features.humanizedPower)
        powerAddition = static_cast<vec_t>(gGlobalVars->features.powerStepSize);
    else
        powerAddition = 0.1;

    if (gGlobalVars->features.humanizedAngleRotation)
        angleAddition = static_cast<vec_t>(gGlobalVars->features.angleRotationStepSize);
    else
        angleAddition = gTimePerTick;

nineBallLabel:
    index = 0;
powerLabel:
    sub_1C2B0D8(&power);
    scaledPower = scalePower(power);
    v29 = -(spinAmount.y * scaledPower) / Ball::getRadius();
    v30 = spinAmount.x * scaledPower / Ball::getRadius();
cushionLabel:
    if (gGlobalVars->features.automatic == 1)
        angle = vecZero;
    else
        angle = actualAngle;

    angleDelta = vecZero;

angleLabel:
    this->initSceneData(data, false);
    sub_1C2B0D8(&angle);
    vec_t v26 = sin(angle);
    vec_t v27 = cos(angle);
    cueBallInfo->velocity.x = scaledPower * v27;
    cueBallInfo->velocity.y = scaledPower * v26;
    vec_t v31 = -(spinAmount.y * scaledPower) / Ball::getRadius();
    vec_t v45 = -(v26 * v31);
    vec_t v46 = v27 * v31;
    vec_t v47 = spinAmount.x * scaledPower / Ball::getRadius();
    cueBallInfo->spin.x = v45;
    cueBallInfo->spin.y = v46;
    cueBallInfo->spin.z = v47;
    data->collision->firstHitBall = nullptr;
    data->validCushionShot = false;

    this->predictFinalDestinations(data, count);
    this->updateState(data, bestIndex, is9BallGame, nineBallGameRule);

Label:
    if ((angleDelta + angleAddition) < maxAngle && data->state == false) {
        angleDelta += angleAddition;
        angle += angleAddition;
        normalizeAngle(angle);
        goto angleLabel;
    }

    if (gGlobalVars->features.automatic == 2) {
        if (gGlobalVars->features.humanizedPower) {
            if (data->state == false && (power + powerAddition) <= 1.0) {
                power += powerAddition;
                goto powerLabel;
            }
        }
    }

    if (data->state) {
        switch (gGlobalVars->features.automatic) {
        case 1:
            if (angle != actualAngle) {
                VisualGuide::setAimAngle(gGlobalVars, visualGuide, angle);
            }
            break;
        case 2:
            if (canNominate) {
                bool result = false;
                for (index = 1; index < data->ballsCount; index++) {
                    predictionInfo = &data->info[index];
                    if (predictionInfo->originalOnTable != predictionInfo->onTable) {
                        if (predictionInfo->classification == myClassification) {
                            if (predictionInfo->pocketIndex == nominatedPocket) {
                                result = true;
                                break;
                            }
                        }
                    }
                }

                if (result == false) {
                    data->state = false;
                    goto Label;
                }
            }

            /*getPocketPositionByIndex(data->info[bestIndex].pocketIndex, clickPos);
            if (canNominatePocket(GameManager::playerClassification(gGlobalVars, 0), is9BallGame, data, true)) {
                do {
                    SendMessage(AEngineRenderWindow::window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(clickPos[0], clickPos[1]));
                    SendMessage(AEngineRenderWindow::window, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(clickPos[0], clickPos[1]));
                    Sleep(500);
                } while (GameManager::getNominatedPocket(gGlobalVars) < 6);
            }*/

            screenCenter = nullVector.toScreen();
            clickPos[0] = static_cast<int>(screenCenter.x);
            clickPos[1] = static_cast<int>(screenCenter.y);
            if (angle != actualAngle) {
                if (gGlobalVars->features.humanizedAngleRotation) {
                    angleDelta2 = vecZero;
                    do {
                        actualAngle += angleAddition;
                        normalizeAngle(actualAngle);
                        sub_1C2B0D8(&actualAngle);
                        VisualGuide::setAimAngle(gGlobalVars, visualGuide, actualAngle);
                        SendMessage(AEngineRenderWindow::window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(clickPos[0], clickPos[1]));
                        Sleep(static_cast<DWORD>(gGlobalVars->features.delayBetweenAngleSteps));
                        SendMessage(AEngineRenderWindow::window, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(clickPos[0], clickPos[1]));
                        if (GameManager::isAnyPopupOpen(gGlobalVars))
                            return;

                        angleDelta2 += angleAddition;
                    } while (angleDelta2 < angleDelta);
                }
                else
                {
                    VisualGuide::setAimAngle(gGlobalVars, visualGuide, angle);
                }
            }

            while (GameManager::isPlayerTurn() == true) {
                this->setPowerAmount(power);
                SendMessage(AEngineRenderWindow::window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(clickPos[0], clickPos[1]));
                SendMessage(AEngineRenderWindow::window, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(clickPos[0], clickPos[1]));
                if (GameManager::isAnyPopupOpen(gGlobalVars) || gGlobalVars->features.automatic == 0)
                    return;

                Sleep(1);
            }

            break;
        }
    }
    else if (nineBallGameRule == 1) {
        nineBallGameRule = 0;
        goto nineBallLabel;
    }
}

void Prediction::initSceneData(SceneData* data, const bool insert)
{
    int             i;
    UINT32          ball;
    PredictionInfo* predictionInfo;

    for (i = 0UL; i < data->ballsCount; i++) {
        ball = Balls::getBall(gGlobalVars, i, data->ballList);
        predictionInfo = &data->info[i];
        predictionInfo->index = i;
        predictionInfo->state = Ball::getState(gGlobalVars, ball);
        predictionInfo->onTable = (predictionInfo->state == 1 || predictionInfo->state == 2);
        predictionInfo->originalOnTable = predictionInfo->onTable;
        predictionInfo->classification = Ball::classification(gGlobalVars, ball);
        predictionInfo->initialPos = Ball::getPosition(gGlobalVars, ball);
        predictionInfo->predictedPosition = predictionInfo->initialPos;
        predictionInfo->velocity.nullify();
        predictionInfo->spin.nullify();

        if (insert)
            predictionInfo->positions.push_back(predictionInfo->initialPos);
    }
}

void Prediction::predictFinalDestinations(SceneData* data, const int& ballsCount)
{
    int             i;
    bool            result;
    vec_t           time, time2;
    PredictionInfo* predictionInfo;

    do
    {
        time = gTimePerTick;
        do
        {
            time2 = time;
            data->collision->valid = false;
            for (i = 0; i < ballsCount; i++) {
                predictionInfo = &data->info[i];
                if (predictionInfo->onTable)
                    predictionInfo->findFirstCollision(data, &time2);
            }

            for (i = 0; i < ballsCount; i++) {
                predictionInfo = &data->info[i];
                result = predictionInfo->isMovingOrSpinning();
                if (result && predictionInfo->onTable) {
                    predictionInfo->move(time2);
                }
            }

            if (data->collision->valid) {
                this->handleCollision(data);
            }

            time -= time2;
        } while (time > 1E-11);

        for (i = 0; i < ballsCount; i++) {
            predictionInfo = &data->info[i];
            if (predictionInfo->onTable) {
                predictionInfo->calcVelocity();
            }
        }

        result = false;
        for (i = 0; i < ballsCount; i++) {
            predictionInfo = &data->info[i];
            result = predictionInfo->onTable && predictionInfo->isMovingOrSpinning();
            if (result)
                break;
        }

    } while (result == true);
}

void Prediction::updateState(SceneData* data, int& bestIndex, bool is9BallGame, bool nineBallGameRule)
{
    int             myClassification, index;
    bool            newState, result;
    PredictionInfo* predictionInfo;

    data->state = false;
    if (data->collision->firstHitBall == nullptr)
        return;

    if (data->info[0].onTable == false)
        return;

    result = false;
    if (GameManager::isPlayerTurn()) {
        myClassification = GameManager::playerClassification(gGlobalVars, 0);
    }
    else
    {
        myClassification = GameManager::playerClassification(gGlobalVars, 1);
    }

    if (is9BallGame) {
        data->state = (data->collision->firstHitBall->classification == myClassification);
        if (data->state == false)
            return;

        if (nineBallGameRule == false) {
            for (index = 1; index < data->ballsCount; index++) {
                predictionInfo = &data->info[index];
                if (predictionInfo->originalOnTable != predictionInfo->onTable) {
                    bestIndex = index;
                    return;
                }
            }

            data->state = false;
        }
        else
        {
            data->state = (data->info[9].originalOnTable != data->info[9].onTable);
            bestIndex = 9;
        }

        return;
    }

    for (index = 1; index < data->ballsCount; index++) {
        predictionInfo = &data->info[index];
        if (predictionInfo->originalOnTable != predictionInfo->onTable) {
            if (index == 8) {
                if (predictionInfo->classification == myClassification) {
                    result = true;
                    bestIndex = index;
                }
                else
                {
                    // state is already false
                    return;
                }
            }
            else if (predictionInfo->classification == myClassification || myClassification == -1) {
                result = true;
                bestIndex = index;
            }
        }
    }

    if (myClassification == -1 && data->collision->firstHitBall->index != 8)
        newState = true;
    else if (data->collision->firstHitBall->classification == myClassification)
        newState = true;
    else
        newState = false;


    data->state = (result && newState);
}


void Prediction::onInitialization()
{
    this->guiData.collision = &collisionA;
    this->autoAimData.collision = &collisionB;
}

static vec_t calcAngle(const Vector2D& source, const Vector2D& Destination)
{
    vec_t    angle;
    Vector2D delta;

    delta = source - Destination;
    angle = calcAngle(delta);

    return angle;
}

static vec_t calcAngle(const Vector2D& delta)
{
    vec_t angle;

    if (delta.x == vecZero) {
        angle = vecPI * 1.5;
        if (delta.y >= vecZero)
            angle = vecPI * 0.5;
    }
    else
    {
        angle = atan(delta.y / delta.x);
        if (delta.x < vecZero)
            angle += vecPI;
    }

    return angle;
}

vec_t Prediction::getPowerAmount()
{
    vec_t  power;
    UINT32 visualCue;

    constexpr vec_t defaultPowerAmount = 1;

    visualCue = gGlobalVars->memory->read<UINT32>(gGlobalVars->sharedGameManager + 0x2D8UL);
    if (visualCue == 0ULL)
        goto failure;

    power = gGlobalVars->memory->read<vec_t>(visualCue + 0x280UL);
    if (power == 0.0) {
    failure:
        power = defaultPowerAmount;
    }

    return power;
}

vec_t scalePower(const vec_t& power)
{
    vec_t maxPower, result;

    maxPower = gGlobalVars->memory->read<vec_t>(gGlobalVars->gameModuleBase + gOffsets::CUE_PROPERTIES_MAX_POWER);
    result = (1.0 - sqrt(1.0 - power * maxPower / maxPower)) * maxPower;

    return result;
}

void Prediction::setPowerAmount(vec_t value)
{
    UINT32 visualCue;

    visualCue = gGlobalVars->memory->read<UINT32>(gGlobalVars->sharedGameManager + 0x2D8UL);
    if (visualCue == 0ULL)
        return;

    gGlobalVars->memory->write(visualCue + 0x280UL, value);
}

Vector2D Prediction::getSpinAmount()
{
    Vector2D spin;
    vec_t    maxSpin;
    UINT32   visualCue, unknownObject;

    constexpr vec_t defaultSpinAmount = vecZero;

    visualCue = gGlobalVars->memory->read<UINT32>(gGlobalVars->sharedGameManager + 0x2D8UL);
    if (visualCue == 0ULL) goto failure;
    unknownObject = gGlobalVars->memory->read<UINT32>(visualCue + 0x1FCUL);
    if (unknownObject == 0ULL) goto failure;
    unknownObject = gGlobalVars->memory->read<UINT32>(unknownObject + 0x2E0UL);
    if (unknownObject == 0ULL) goto failure;

    gGlobalVars->memory->read(unknownObject + 0x298UL, &spin, sizeof(spin));
    goto success;

failure:
    spin.x = spin.y = defaultSpinAmount;

success:
    maxSpin = gGlobalVars->memory->read<vec_t>(gGlobalVars->gameModuleBase + gOffsets::CUE_PROPERTIES_SPIN);
    spin.x *= maxSpin;
    spin.y *= maxSpin;

    return spin;
}

bool Prediction::PredictionInfo::isMovingOrSpinning()
{
    return ((this->velocity.isNull() && this->spin.isNull()) == false);
}