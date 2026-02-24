#ifndef FOOTBALL_H
#define FOOTBALL_H

// Weather class
class Weather {
public:
    float visibilityModifier; // Modifier for visibility conditions from 0 to 1
    float frictionCoefficient; // Coefficient for how slippery the field is from 0 to 1
    float getVisibilityModifier();
    float getFrictionCoefficient();
};

// Types of weather: (normal, rain, fog)
class Normal : public Weather {
public:
    Normal();
};

class Rain : public Weather {
public:
    Rain();
    float moistureLevel; // Level of moisture on the field from 0 to 1
};

class Fog : public Weather {
public:
    Fog();
};

// PlayerAccidents class
class PlayerAccidents {
public:
    float injuryProbability; // Probability of player injury from 0 to 1
    float fatigueLevel;      // Level of player fatigue from 0 to 1
    float getInjuryProbability();
    float getFatigueLevel();
};

// Types of player accidents (PlayerSlips, BallDrops)
class PlayerSlips : public PlayerAccidents {
public:
    PlayerSlips();
};

class BallDrops : public PlayerAccidents {
public:
    BallDrops();
};
#endif