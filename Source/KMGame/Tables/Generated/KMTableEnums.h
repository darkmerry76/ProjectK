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
	Beast = 3 , // 비스트
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

// 스킬이펙트거리타잎
UENUM(BlueprintType)
enum class EKMSkillTransitionBehavior : uint8
{
	None = 0 , // 없음
	Disable = 1 , // 기능멈춤
	Remove = 2 , // 삭제
	Suspend = 3 , // 일시정지
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

// 스킬효과타잎
UENUM(BlueprintType)
enum class EKMSkillEffectOverlapType : uint8
{
	None = 0 , // 없음
	Add = 1 , // 증가
	Override = 2 , // 덮어쓰기
	Refresh = 3 , // Duration갱신
	Ignore = 4 , // 무시
	Max = 5 , // 더강한기준
	Min = 6 , // 더약한기준
	Remove = 7 , // 기존효과제거
	Multiply = 8 , // 배율
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
enum class EKMLocomotionState : uint8
{
	None = 0 , // 없음
	Land = 1 , 
	Air = 2 , 
	Falling = 3 , 
};

UENUM(BlueprintType)
enum class EKMFightStyleType : uint8
{
	None = 0 , // 없음
	Berserker = 1 , 
	Assassin = 2 , 
};

UENUM(BlueprintType)
enum class EKMCoreArmamentType : uint8
{
	None = 0 , 
	Chain = 1 , 
	Kunai = 2 , 
};

UENUM(BlueprintType)
enum class EKMDamagePowerType : uint8
{
	None = 0 , 
	Default = 1 , 
	Low = 2 , 
	Medium = 3 , 
	High = 4 , 
};

UENUM(BlueprintType)
enum class EKMTargetLockonType : uint8
{
	None = 0 , 
	Lockon = 1 , 
	Stand = 2 , 
	Down = 3 , 
	Air = 4 , 
};
