#ifndef VTABLE_LIB_H
#define VTABLE_LIB_H

#include <stdio.h>

typedef struct Gladiator Gladiator;

typedef struct {
    void (*attack)(Gladiator *g);
} GladiatorVTable;

struct Gladiator {
    char name[40];
    char pwd[40];
    char comment[40];
    GladiatorVTable *vtable;
};

typedef struct {
    Gladiator current;
} GlobalData;

extern GlobalData globals __attribute__((section(".current_data")));

#define current (globals.current)

void defaultAttack(Gladiator *g);
void StrongAttack(Gladiator *g);
void ArrowAttack(Gladiator *g);
void NotdefaultAttack(Gladiator *g);
void NuclearAttack(Gladiator *g);
void RocketAttack(Gladiator *g);
void NotAttack(Gladiator *g);
void FireAttack(Gladiator *g);
void IceAttack(Gladiator *g);
void LightningAttack(Gladiator *g);
void HealingAction(Gladiator *g);
void BerserkAttack(Gladiator *g);
void ShieldDefense(Gladiator *g);
void WaterAttack(Gladiator *g);
void EarthAttack(Gladiator *g);
void WindAttack(Gladiator *g);
void PoisonAttack(Gladiator *g);
void MagicShield(Gladiator *g);
void StealthAction(Gladiator *g);
void ThunderStrike(Gladiator *g);
void ShadowAttack(Gladiator *g);
void EarthquakeAttack(Gladiator *g);
void RapidStrike(Gladiator *g);
void CounterAttack(Gladiator *g);
void ChargeAttack(Gladiator *g);
void MagicMissile(Gladiator *g);
void DivineSmite(Gladiator *g);
void SummonGolem(Gladiator *g);
void BattleCry(Gladiator *g);

void hello_world(void);

extern GladiatorVTable default_vtable;

#endif
