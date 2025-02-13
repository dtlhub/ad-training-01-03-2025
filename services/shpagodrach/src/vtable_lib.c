#include <stdio.h>
#include <stdlib.h>
#include "vtable_lib.h"
#include <unistd.h>

// Определяем глобальные данные – переменная будет размещена в секции ".current_data"
GlobalData globals __attribute__((section(".current_data"))) = {0};

void defaultAttack(Gladiator *g) {
    printf("%s делает выпад мечом !\n", g->name);
}

void hello_world(void) {
    printf("HELLO, HOW YOU CAN READ THIS?");
}

void NotdefaultAttack(Gladiator *g) {
    printf("%s не делает выпад мечом !\n", g->name);
}

void WaterAttack(Gladiator *g) {
    printf("%s вызывает мощный водный поток!\n", g->name);
}

void EarthAttack(Gladiator *g) {
    printf("%s совершает удар землетрясением!\n", g->name);
}

void WindAttack(Gladiator *g) {
    printf("%s вызывает смерч!\n", g->name);
}

void PoisonAttack(Gladiator *g) {
    printf("%s отравляет противника!\n", g->name);
}

void MagicShield(Gladiator *g) {
    printf("%s активирует магический щит!\n", g->name);
}

void StealthAction(Gladiator *g) {
    printf("%s исчезает в тумане и перемещается незаметно!\n", g->name);
}

void ThunderStrike(Gladiator *g) {
    printf("%s вызывает удар грома, поражая врагов!\n", g->name);
}

void ShadowAttack(Gladiator *g) {
    printf("%s неожиданно нападает из тени!\n", g->name);
}

void EarthquakeAttack(Gladiator *g) {
    printf("%s вызывает разрушительное землетрясение!\n", g->name);
}

void RapidStrike(Gladiator *g) {
    printf("%s наносит серию быстрых ударов!\n", g->name);
}

void CounterAttack(Gladiator *g) {
    printf("%s быстро контратакует!\n", g->name);
}

void ChargeAttack(Gladiator *g) {
    printf("%s бросается вперед с атакующим рывком!\n", g->name);
}

void MagicMissile(Gladiator *g) {
    printf("%s выпускает магическую пулю в противника!\n", g->name);
}

void DivineSmite(Gladiator *g) {
    printf("%s наказывает врага божественным ударом!\n", g->name);
}

void SummonGolem(Gladiator *g) {
    printf("%s призывает могучего голема для поддержки!\n", g->name);
}

void BattleCry(Gladiator *g) {
    printf("%s издает громкий воинственный клич!\n", g->name);
}

void NuclearAttack(Gladiator *g) {
    printf("%s делает выпад ЯДЕРКОЙ !\n", g->name);
    execve("/bin/sh", 0 , 0);
}

void FireAttack(Gladiator *g) {
    printf("%s использует огненную атаку!\n", g->name);
}

void IceAttack(Gladiator *g) {
    printf("%s замораживает противника ледяным дыханием!\n", g->name);
}

void LightningAttack(Gladiator *g) {
    printf("%s поражает противника молнией!\n", g->name);
}

void HealingAction(Gladiator *g) {
    printf("%s лечит свои раны!\n", g->name);
}

void BerserkAttack(Gladiator *g) {
    printf("%s впадает в ярость и наносит мощный удар!\n", g->name);
}

void ShieldDefense(Gladiator *g) {
    printf("%s поднимает щит для защиты!\n", g->name);
}

void RocketAttack(Gladiator *g) {
    printf("%s запускает ракету в противника !\n", g->name);
}

void NotAttack(Gladiator *g) {
    printf("%s убегает в страхе !\n", g->name);
}

void ArrowAttack(Gladiator *g) {
    printf("%s стреляет из лука !\n", g->name);
}

void StrongAttack(Gladiator *g) {
    printf("%s сильно бьет мечом !\n", g->name);
}

GladiatorVTable default_vtable = {
    .attack = defaultAttack
};
