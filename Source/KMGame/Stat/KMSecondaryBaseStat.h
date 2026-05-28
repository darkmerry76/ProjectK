#pragma once
#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "KMSecondaryBaseStat.generated.h"

UENUM(BlueprintType)
enum class EKMStatFactorType : uint8
{
	Hp,
	HpRps,
	HpCurr,
	Sp,
	SpRps,
	SpCurr,
	Mp,
	MpRps,
	MpCurr,
	Atk,
	AtkSpd,
	Dex,
	Def,
	CriChange,
	Cri,
	Mov,
	Run,
	Emp,
	EmpPhase,
	EmpPhaseCurr,
	EmpCurr,
	Tempo,
	TempoRps,
	TempoCurr,
	Damage,
	DamageCri,
	DamageDot,
	DamageMax,
	GoodCancel,
	GreatCancel,
	PerfectCancel,
	GoodParry,
	GreatParry,
	PerfectParry,
	Max,
};

DECLARE_MULTICAST_DELEGATE_ThreeParams(FKMStatChange, EKMStatFactorType factorType, float prevValue, float newValue)
class KMGAME_API FKMBaseStat
{
public:
	virtual void Init(const struct FKMTable_BaseStatRow* baseStatTable);

	void Multiply(const struct FKMTable_Stat_InLevelUpRow* inLevelTable);
	void Multiply(const struct FKMTable_Stat_OutLevelUpRow* inLevelTable);
	void Multiply(const struct FKMTable_PlayerLevelRow* playerLvelTable);
	
	virtual FName GetType() const { return FKMBaseStat::GetTypeName(); }
	static FName GetTypeName() { return TEXT("FKMBaseStat"); }
	
	virtual ~FKMBaseStat() { }

	double GetHp() const { return Hp; }
	void SetHp(double newHp, bool bBroadcast = false);

	double GetHpRps() const { return Hp; }
	void SetHpRps(double newHpRps, bool bBroadcast = false);

	double GetSp() const { return Sp; }
	void SetSp(double newSp, bool bBroadcast = false);

	double GetSpRps() const { return SpRps; }
	void SetSpRps(double newSpRps, bool bBroadcast = false);

	double GetMp() const { return Mp; }
	void SetMp(double newMp, bool bBroadcast = false);

	double GetMpRps() const { return MpRps; }
	void SetMpRps(double newMpRps, bool bBroadcast = false);

	double GetAtk() const { return Atk; }
	void SetAtk(double newAtk, bool bBroadcast = false);

	double GetAtkSpd() const { return AtkSpd; }
	void SetAtkSpd(double newAtkSpd, bool bBroadcast = false);
	
	double GetDex() const { return Dex; }
	void SetDex(double newDex, bool bBroadcast = false);
	
	double GetDef() const { return Def; }
	void SetDef(double newDef, bool bBroadcast = false);
	
	double GetCriChange() const { return CriChance; }
	void SetCriChange(double newCriChange, bool bBroadcast = false);
	
	double GetCri() const { return Cri; }
	void SetCri(double newCriChange, bool bBroadcast = false);
	
	double GetMov() const { return Mov; }
	void SetMov(double newMov, bool bBroadcast = false);
	
	double GetRun() const { return Run; }
	void SetRun(double newRun, bool bBroadcast = false);

	double GetEmp() const { return Emp; }
	void SetEmp(double newEmp, bool bBroadcast = false);

	int32 GetEmpPhase() const { return EmpPhase; }
	void SetEmpPhase(int32 newEmpPhase, bool bRoadcast = false);

	double GetTempo() const { return Tempo; }
	void SetTempo(double newTempo, bool bBroadcast = false);

	double GetTempoRps() const { return TempoRps; }
	void SetTempoRps(double newTempoRps, bool bBroadcast = false);

	double GetGoodCancel() const { return GoodCancel; }
	double GetGreatCancel() const { return GreatCancel; }
	double GetPerfectCancel() const { return PerfectCancel; }
	
	double GetGoodParry() const { return GoodParry; }
	double GetGreatParry() const { return GreatParry; }
	double GetPerfectParry() const { return PerfectParry; }
	
	virtual void Sanitization() { }
	virtual void DuplicateValue(const FKMBaseStat* otherStat);
	
	FKMStatChange StatChange;

protected:
	double Hp = 100.f;
	double HpRps = 1.f;
	
	double Sp = 100.f;
	double SpRps = 1.f;

	double Mp = 100.f;
	double MpRps = 1.f;
	
	double Atk = 20.f;
	double AtkSpd = 1.f;

	double Def = 0.f;
	double Dex = 100.f;
	
	double CriChance = 10.f;
	double Cri = 20.f;
	double Mov = 100.f;
	double Run = 200.f;

	double Emp = 5.f;
	int32 EmpPhase = 0;

	double Tempo = 6.f;
	double TempoRps = 0.2f;

	double GoodCancel = FRAME_TO_SECONDS(15);
	double GreatCancel = FRAME_TO_SECONDS(7);
	double PerfectCancel = FRAME_TO_SECONDS(4);

	double GoodParry = FRAME_TO_SECONDS(0);
	double GreatParry = FRAME_TO_SECONDS(6);
	double PerfectParry = FRAME_TO_SECONDS(3);
};

class KMGAME_API FKMSecondaryStat : public FKMBaseStat
{
public:
	virtual void Init(const struct FKMTable_BaseStatRow* baseStatTable) override;
	
	virtual FName GetType() const override { return FKMSecondaryStat::GetTypeName(); }
	static FName GetTypeName() { return TEXT("FKMSecondaryStat"); }
	
	double GetCooltimeWeights() const;

	void ApplyDamage(double damage , bool bBroadcast);

	double GetHpCurr() const { return HpCurr; }
	void SetHpCurr(double newCurrentHp, bool bBroadcast = false);

	double GetSpCurr() const { return SpCurr; }
	void SetSpCurr(double newSpCurr, bool bBroadcast);

	double GetMpCurr() const { return MpCurr; }
	void SetMpCurr(double newCurrentHp, bool bBroadcast = false);

	double GetEmpCurr() const { return EmpCurr; }
	void SetEmpCurr(double newEmpCurr, bool bBroadcast = false);
	
	int32 GetEmpPhaseCurr() const { return EmpPhaseCurr; }
	void SetEmpPhaseCurr(int32 newEmpPhaseCurr, bool bBroadcast = false);

	double GetTempoCurr() const { return TempoCurr; }
	void SetTempoCurr(double newTempoCurr, bool bBroadcast = false);
	
	void BroadcastDamage(double damage);
	void BroadcastCriDamage(double damage);
	void BroadcastDotDamage(double damage);
	
	virtual void Sanitization() override;
	virtual void DuplicateValue(const FKMBaseStat* otherStat) override;
	
protected:
	double HpCurr = 100.f;
	double MpCurr = 100.f;
	double SpCurr = 100.f;
	double EmpCurr = 0.f;
	double TempoCurr = 6.f;
	int32 EmpPhaseCurr = 0;
};