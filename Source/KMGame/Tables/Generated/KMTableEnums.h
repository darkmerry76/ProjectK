#pragma once

#include "CoreMinimal.h"
#include "KMTableEnums.generated.h"


// 역할
UENUM(BlueprintType)
enum class EKMCharacterType : uint8
{
	Hero = 0 , // 플레이어
	Npc = 1 , // NPC
	Monster = 2 , // 몬스터
};

// 역할
UENUM(BlueprintType)
enum class EKMCharacterBattleRole : uint8
{
	None = 0 , // 미지정
	Tanker = 1 , // 탱커
	Dealer = 2 , // 딜러
	Supporter = 3 , // 지원
	Max = 4 , 
};

// 캐릭터 위치 타입(StageSpawn,Character)에서 사용중
UENUM(BlueprintType)
enum class EKMCharacterLocationType : uint8
{
	None = 0 , // 미지정
	Ground = 1 , // 지상
	Fly = 2 , // 공중
	Underwater = 3 , // 물속
	Climb = 4 , // 매달리는(나무,절벽)
};

// 히어로 등급 에서 사용
UENUM(BlueprintType)
enum class EKMHeroGrade : uint8
{
	None = 0 , // 미지정
	Common = 1 , // 일반 (회색)
	Rare = 2 , // 레어 (파랑)
	Epic = 3 , // 에픽 (에픽)
	Legendary = 4 , // 전설 (금색)
};

// 몬스터 등급 에서 사용
UENUM(BlueprintType)
enum class EKMMonsterGrade : uint8
{
	None = 0 , // 미지정
	Normal = 1 , // 일반
	Elite = 2 , // 정예
	Boss = 3 , // 보스
};

// 스킬타잎
UENUM(BlueprintType)
enum class EKMSkillType : uint8
{
	None = 0 , // 없음
	Active = 1 , // 조작발동
	Passive = 2 , // 자동발동
};

// 스킬 발동타잎
UENUM(BlueprintType)
enum class EKMSkillActiveType : uint8
{
	None = 0 , // 없음
	Instant = 1 , // 즉시발동
	Charging = 2 , // 모아서 쓰기
	Cahnneling = 3 , // 유지
	Parry = 4 , // 반격
	Toggle = 5 , // 토글
	Interval = 6 , // 주기적
};

// 스킬 적용대상
UENUM(BlueprintType)
enum class EKMSkillTargetType : uint8
{
	None = 0 , // 없음
	Self = 1 , // 자신
	Hero = 2 , // 히어로
	Monster = 3 , // 악귀
	Ghost = 4 , // 선령
};

// 스킬 적용대상
UENUM(BlueprintType)
enum class EKMSkillEffectTargetType : uint8
{
	None = 0 , // 없음
	Instigator = 1 , // 가해자
	Target = 2 , // 피해자
};

// 스킬적용대상 범위
UENUM(BlueprintType)
enum class EKMSkillScopeType : uint8
{
	None = 0 , // 없음
	Single = 1 , // 하나의 대상
	Multiple = 2 , // 여러대상
	Area = 3 , // 구역
	Chain = 4 , // 연결
};

// 스킬효과타잎
UENUM(BlueprintType)
enum class EKMSkillEffectType : uint8
{
	None = 0 , // 없음
	Damage = 1 , // 공격
	Buff = 2 , // 버프
	Abnormal = 3 , // 상태이상
	Gauge = 4 , // 게이지
};

// 스킬이펙트거리타잎
UENUM(BlueprintType)
enum class EKMSkillRangeType : uint8
{
	Single = 0 , // 락온대상
	Party = 1 , // 파티
	Area = 2 , // 광역
};

// 스킬이펙트연산자
UENUM(BlueprintType)
enum class EKMOperatorType : uint8
{
	None = 0 , // 없음
	Add = 1 , // 더하기
	Subtract = 2 , // 빼기
	Multiply = 3 , // 곱하기
	Divide = 4 , // 나누기
};

// 에니메이션
UENUM(BlueprintType)
enum class EKMAnimType : uint8
{
	None = 0 , // 없음
	Spawn = 1 , // 스폰
	Idle = 2 , // 아이들
	IdleSpecial = 3 , // 지겨울때나오는 아이들
	Attack = 4 , // 공격
	Walk = 5 , // 이동
	Hit = 6 , // 맞았을때
	Death = 7 , // 죽었을때
};

// 리워드타잎
UENUM(BlueprintType)
enum class EKMRewardType : uint8
{
	None = 0 , // 없음
	Exp = 1 , // 경험치
	Gold = 2 , // 골드
	Collect = 3 , // 수집
};

UENUM(BlueprintType)
enum class EKMStatParameterType : uint8
{
	None = 0 , // 없음
	Hp = 1 , 
	HpRps = 2 , 
	HpCurr = 3 , 
	Sp = 4 , 
	SpRps = 5 , 
	SpCurr = 6 , 
	Mp = 7 , 
	MpRps = 8 , 
	MpCurr = 9 , 
	Atk = 10 , 
	AtkSpd = 11 , 
	Dex = 12 , 
	Def = 13 , 
	CriChance = 14 , 
	Cri = 15 , 
	Mov = 16 , 
	Run = 17 , 
	Emp = 18 , 
	EmpPhase = 19 , 
	EmpPhaseCurr = 20 , 
	EmpCurr = 21 , 
	Tempo = 22 , 
	TempoRps = 23 , 
	TempoCurr = 24 , 
};

UENUM(BlueprintType)
enum class EKMAnimSetSkillType : uint8
{
	None = 0 , // 없음
	Stand_Guard_0 = 1 , 
	Stand_Guard_1 = 2 , 
	Stand_Guard_2 = 3 , 
	Jump_0 = 10 , 
	Landing_0 = 20 , 
	Front_Dash_0 = 30 , 
	Front_Dash_1 = 31 , 
	Front_Dash_2 = 32 , 
	Back_Dash_0 = 33 , 
	Back_Dash_1 = 34 , 
	Back_Dash_2 = 35 , 
	Front_Good_Cancel_Dash_0 = 36 , 
	Front_Good_Cancel_Dash_1 = 37 , 
	Front_Good_Cancel_Dash_2 = 38 , 
	Front_Great_Cancel_Dash_0 = 39 , 
	Front_Great_Cancel_Dash_1 = 40 , 
	Front_Great_Cancel_Dash_2 = 41 , 
	Front_Perfect_Cancel_Dash_0 = 42 , 
	Front_Perfect_Cancel_Dash_1 = 43 , 
	Front_Perfect_Cancel_Dash_2 = 44 , 
	Back_Good_Cancel_Dash_0 = 45 , 
	Back_Good_Cancel_Dash_1 = 46 , 
	Back_Good_Cancel_Dash_2 = 47 , 
	Back_Great_Cancel_Dash_0 = 48 , 
	Back_Great_Cancel_Dash_1 = 49 , 
	Back_Great_Cancel_Dash_2 = 50 , 
	Back_Perfect_Cancel_Dash_0 = 51 , 
	Back_Perfect_Cancel_Dash_1 = 52 , 
	Back_Perfect_Cancel_Dash_2 = 53 , 
	Front_Good_Parry_0 = 54 , 
	Front_Good_Parry_1 = 55 , 
	Front_Good_Parry_2 = 56 , 
	Front_Great_Parry_0 = 57 , 
	Front_Great_Parry_1 = 58 , 
	Front_Great_Parry_2 = 59 , 
	Front_Perfect_Parry_0 = 60 , 
	Front_Perfect_Parry_1 = 61 , 
	Front_Perfect_Parry_2 = 62 , 
	Back_Good_Parry_0 = 63 , 
	Back_Good_Parry_1 = 64 , 
	Back_Good_Parry_2 = 65 , 
	Back_Great_Parry_0 = 66 , 
	Back_Great_Parry_1 = 67 , 
	Back_Great_Parry_2 = 68 , 
	Back_Perfect_Parry_0 = 69 , 
	Back_Perfect_Parry_1 = 70 , 
	Back_Perfect_Parry_2 = 71 , 
	Front_Deflect_0 = 72 , 
	Front_Deflect_1 = 73 , 
	Front_Deflect_2 = 74 , 
	Init_0 = 75 , 
	Init_1 = 76 , 
	Init_2 = 77 , 
	Stand_Low_Kick = 100 , 
	Stand_Middle_Kick = 101 , 
	Stand_High_Kick = 102 , 
	Crouch_LowKick = 103 , 
	Crouch_Middle_Kick = 104 , 
	Crouch_High_Kick = 105 , 
	Air_Low_Kick = 106 , 
	Air_Middle_Kick = 107 , 
	Air_High_Kick = 108 , 
};

UENUM(BlueprintType)
enum class EKMAnimSetEffectType : uint8
{
	None = 0 , // 없음
	Stand_Hit_0 = 1 , 
	Stand_Hit_1 = 2 , 
	Stand_Hit_2 = 3 , 
	Air_Hit_0 = 4 , 
	Air_Hit_1 = 5 , 
	Air_Hit_2 = 6 , 
	Crouch_Hit_0 = 7 , 
	Crouch_Hit_1 = 8 , 
	Crouch_Hit_2 = 9 , 
	Stand_Blow_Front_0 = 20 , 
	Stand_Blow_Front_1 = 21 , 
	Stand_Blow_Front_2 = 22 , 
	Stand_Blow_Back_0 = 23 , 
	Stand_Blow_Back_1 = 24 , 
	Stand_Blow_Back_2 = 25 , 
	Stand_Spin_Front_0 = 26 , 
	Stand_Spin_Front_1 = 27 , 
	Stand_Spin_Front_2 = 28 , 
	Stand_Spin_Back_0 = 29 , 
	Stand_Spin_Back_1 = 30 , 
	Stand_Spin_Back_2 = 31 , 
	Stand_WallBlow_Front_0 = 32 , 
	Stand_WallBlow_Front_1 = 33 , 
	Stand_WallBlow_Front_2 = 34 , 
	Stand_WallBlow_Back_0 = 35 , 
	Stand_WallBlow_Back_1 = 36 , 
	Stand_WallBlow_Back_2 = 37 , 
	Stand_GroundBlow_Front_0 = 38 , 
	Stand_GroundBlow_Front_1 = 39 , 
	Stand_GroundBlow_Front_2 = 40 , 
	Stand_GroundBlow_Back_0 = 41 , 
	Stand_GroundBlow_Back_1 = 42 , 
	Stand_GroundBlow_Back_2 = 43 , 
	Bound_Front_0 = 44 , 
	Bound_Front_1 = 45 , 
	Bound_Front_2 = 46 , 
	Bound_Back_0 = 47 , 
	Bound_Back_1 = 48 , 
	Bound_Back_2 = 49 , 
	Bound_Wall_Front_0 = 50 , 
	Bound_Wall_Front_1 = 51 , 
	Bound_Wall_Front_2 = 52 , 
	Bound_Wall_Back_0 = 53 , 
	Bound_Wall_Back_1 = 54 , 
	Bound_Wall_Back_2 = 55 , 
	WallBlow_Falling_Front_0 = 56 , 
	WallBlow_Falling_Front_1 = 57 , 
	WallBlow_Falling_Front_2 = 58 , 
	WallBlow_Falling_Back_0 = 59 , 
	WallBlow_Falling_Back_1 = 60 , 
	WallBlow_Falling_Back_2 = 61 , 
	ThrowBlow_Front_0 = 62 , 
	ThrowBlow_Front_1 = 63 , 
	ThrowBlow_Front_2 = 64 , 
	ThrowBlow_Back_0 = 65 , 
	ThrowBlow_Back_1 = 66 , 
	ThrowBlow_Back_2 = 67 , 
	Down_Front_0 = 68 , 
	Down_Front_1 = 69 , 
	Down_Front_2 = 70 , 
	Down_Back_0 = 71 , 
	Down_Back_1 = 72 , 
	Down_Back_2 = 73 , 
	Getup_Front_0 = 74 , 
	Getup_Front_1 = 75 , 
	Getup_Front_2 = 76 , 
	Getup_Back_0 = 77 , 
	Getup_Back_1 = 78 , 
	Getup_Back_2 = 79 , 
	Die_0 = 200 , 
	Die_1 = 201 , 
};

UENUM(BlueprintType)
enum class EKMLocomotionState : uint8
{
	None = 0 , // 없음
	Land = 1 , 
	Air = 2 , 
	Falling = 3 , 
};
