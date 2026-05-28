#include "KMSecondaryBaseStat.h"

#include "Tables/Generated/KMTable_PlayerLevel.h"
#include "Tables/Generated/KMTable_Stat_InLevelUp.h"
#include "Tables/Generated/KMTable_Stat_OutLevelUp.h"
#include "Tables/Generated\KMTable_BaseStat.h"

void FKMBaseStat::Init(const FKMTable_BaseStatRow* baseStatTable)
{
	check(baseStatTable != nullptr);
	
	Hp = baseStatTable->Hp;
	HpRps = baseStatTable->HpRps;

	Sp = baseStatTable->Sp;
	SpRps = baseStatTable->SpRps;
	
	Mp = baseStatTable->Mp;
	MpRps = baseStatTable->MpRps;
	
	Atk = baseStatTable->Atk;
	AtkSpd = baseStatTable->AtkSpd;
	
	Dex = baseStatTable->Dex;
	Def = baseStatTable->Def;
	Cri = baseStatTable->Cri;
	CriChance = baseStatTable->CriChance;
	Mov = baseStatTable->Mov;
	Run = baseStatTable->Run;

	Emp = baseStatTable->Emp;
	EmpPhase = baseStatTable->EmpPhase;

	Tempo = baseStatTable->Tempo;
	TempoRps = baseStatTable->TempoRps;
}

void FKMBaseStat::Multiply(const FKMTable_Stat_InLevelUpRow* inLevelupTable)
{
	check(inLevelupTable != nullptr);
	
	Hp *= inLevelupTable->Hp;
	HpRps *= inLevelupTable->HpRps;
	
	Sp *= inLevelupTable->Sp;
	SpRps *= inLevelupTable->SpRps;
	
	Atk *= inLevelupTable->Atk;
	AtkSpd *= inLevelupTable->Aspd;
	
	Mp *= inLevelupTable->Mp;
	MpRps *= inLevelupTable->MpRps;
	
	Dex *= inLevelupTable->Dex;
	Def *= inLevelupTable->Def;
	Cri *= inLevelupTable->Cri;
	CriChance *= inLevelupTable->CriChange;
	Mov *= inLevelupTable->Mov;
	Run *= inLevelupTable->Run;

	Emp *= inLevelupTable->Emp;
	EmpPhase *= inLevelupTable->EmpPhase;

	Tempo *= inLevelupTable->Tempo;
	TempoRps *= inLevelupTable->TempoRps;
}

void FKMBaseStat::Multiply(const FKMTable_Stat_OutLevelUpRow* outLevelTable)
{
	check(outLevelTable != nullptr);
	
	Hp *= outLevelTable->Hp;
	HpRps *= outLevelTable->HpRps;

	Sp *= outLevelTable->Sp;
	SpRps *= outLevelTable->SpRps;

	Atk *= outLevelTable->Atk;
	AtkSpd *= outLevelTable->Aspd;
	
	Mp *= outLevelTable->Mp;
	MpRps *= outLevelTable->MpRps;
	
	Dex *= outLevelTable->Dex;
	Def *= outLevelTable->Def;
	Cri *= outLevelTable->Cri;
	CriChance *= outLevelTable->CriChange;
	Mov *= outLevelTable->Mov;
	Run *= outLevelTable->Run;

	Emp *= outLevelTable->Emp;
	EmpPhase *= outLevelTable->EmpPhase;

	Tempo *= outLevelTable->Tempo;
	TempoRps *= outLevelTable->TempoRps;
}

void FKMBaseStat::Multiply(const FKMTable_PlayerLevelRow* playerLvelTable)
{
	check(playerLvelTable != nullptr);
	
	Hp *= playerLvelTable->Hp;
	HpRps *= playerLvelTable->HpRps;

	Sp *= playerLvelTable->Sp;
	SpRps *= playerLvelTable->SpRps;
	
	Atk *= playerLvelTable->Atk;
	AtkSpd *= playerLvelTable->Aspd;
	
	Mp *= playerLvelTable->Mp;
	MpRps *= playerLvelTable->MpRps;
	
	Dex *= playerLvelTable->Dex;
	Def *= playerLvelTable->Def;
	Cri *= playerLvelTable->Cri;
	CriChance *= playerLvelTable->CriChange;
	Mov *= playerLvelTable->Mov;
	Run *= playerLvelTable->Run;

	Emp *= playerLvelTable->Emp;
	EmpPhase *= playerLvelTable->EmpPhase;

	Tempo *= playerLvelTable->Tempo;
	TempoRps *= playerLvelTable->TempoRps;
}

void FKMBaseStat::DuplicateValue(const FKMBaseStat* otherStat)
{
	Hp = otherStat->Hp;
	HpRps = otherStat->HpRps;

	Sp = otherStat->Sp;
	SpRps = otherStat->SpRps;

	Mp = otherStat->Mp;
	MpRps = otherStat->MpRps;
	
	Atk = otherStat->Atk;
	AtkSpd = otherStat->AtkSpd;
	
	Dex = otherStat->Dex;
	
	Def = otherStat->Def;
	CriChance = otherStat->CriChance;
	Cri = otherStat->Cri;
	Mov = otherStat->Mov;
	Run = otherStat->Run;

	Emp = otherStat->Emp;
	EmpPhase = otherStat->EmpPhase;

	Tempo = otherStat->Tempo;
	TempoRps = otherStat->TempoRps;
}

void FKMBaseStat::SetHp(double newHp, bool bBroadcast)
{
	if (bBroadcast == true && Hp != newHp)
	{
		StatChange.Broadcast(EKMStatFactorType::Hp, Hp, newHp);
	}
	Hp = newHp;
}

void FKMBaseStat::SetHpRps(double newHpRps, bool bBroadcast)
{
	if (bBroadcast == true && HpRps != newHpRps)
	{
		StatChange.Broadcast(EKMStatFactorType::HpRps, HpRps, newHpRps);
	}
	HpRps = newHpRps;
}

void FKMBaseStat::SetSp(double newSp, bool bBroadcast)
{
	if (bBroadcast == true && Sp != newSp)
	{
		StatChange.Broadcast(EKMStatFactorType::Sp, Sp, newSp);
	}
	Sp = newSp;
}

void FKMBaseStat::SetSpRps(double newSpRps, bool bBroadcast)
{
	if (bBroadcast == true && SpRps != newSpRps)
	{
		StatChange.Broadcast(EKMStatFactorType::SpRps, SpRps, newSpRps);
	}
	SpRps = newSpRps;
}

void FKMBaseStat::SetMp(double newMp, bool bBroadcast)
{
	if (bBroadcast == true && Mp != newMp)
	{
		StatChange.Broadcast(EKMStatFactorType::Mp, Mp, newMp);
	}
	Mp = newMp;
}

void FKMBaseStat::SetMpRps(double newMpRps, bool bBroadcast)
{
	if (bBroadcast == true && MpRps != newMpRps)
	{
		StatChange.Broadcast(EKMStatFactorType::MpRps, MpRps, newMpRps);
	}
	MpRps = newMpRps;
}

void FKMBaseStat::SetAtk(double newAtk, bool bBroadcast)
{
	if (bBroadcast == true && Atk != newAtk)
	{
		StatChange.Broadcast(EKMStatFactorType::Atk, Atk, newAtk);
	}
	Atk = newAtk;
}

void FKMBaseStat::SetAtkSpd(double newAtkSpd, bool bBroadcast)
{
	if (bBroadcast == true && AtkSpd != newAtkSpd)
	{
		StatChange.Broadcast(EKMStatFactorType::AtkSpd, AtkSpd, newAtkSpd);
	}
	AtkSpd = newAtkSpd;
}
	
void FKMBaseStat::SetDex(double newDex, bool bBroadcast)
{
	if (bBroadcast == true && Dex != newDex)
	{
		StatChange.Broadcast(EKMStatFactorType::Dex, Dex, newDex);
	}
	Dex = newDex;
}

void FKMBaseStat::SetDef(double newDef, bool bBroadcast)
{
	if (bBroadcast == true && Def != newDef)
	{
		StatChange.Broadcast(EKMStatFactorType::Def, Def, newDef);
	}
	Def = newDef;
}
	
void FKMBaseStat::SetCriChange(double newCriChange, bool bBroadcast)
{
	if (bBroadcast == true && !FMath::IsNearlyEqual(CriChance, newCriChange))
	{
		StatChange.Broadcast(EKMStatFactorType::CriChange, CriChance, newCriChange);
	}
	CriChance = newCriChange;
}
	
void FKMBaseStat::SetCri(double newCri, bool bBroadcast)
{
	if (bBroadcast == true && Cri != newCri)
	{
		StatChange.Broadcast(EKMStatFactorType::Cri, Cri, newCri);
	}
	Cri = newCri;
}
	
void FKMBaseStat::SetMov(double newMov, bool bBroadcast)
{
	if (bBroadcast == true && Mov != newMov)
	{
		StatChange.Broadcast(EKMStatFactorType::Mov, Mov, newMov);
	}
	Mov = newMov;
}

void FKMBaseStat::SetRun(double newRun, bool bBroadcast)
{
	if (bBroadcast == true && Run != newRun)
	{
		StatChange.Broadcast(EKMStatFactorType::Run, Run, newRun);
	}
	Run = newRun;
}

void FKMBaseStat::SetEmp(double newEmp, bool bBroadcast)
{
	if (bBroadcast == true && Emp != newEmp)
	{
		StatChange.Broadcast(EKMStatFactorType::Emp, Emp, newEmp);
	}
	Emp = newEmp;
}

void FKMBaseStat::SetEmpPhase(int32 newEmpPhase, bool bBroadcast)
{
	if (bBroadcast == true && EmpPhase != newEmpPhase)
	{
		StatChange.Broadcast(EKMStatFactorType::EmpPhase, EmpPhase, newEmpPhase);
	}
	EmpPhase = newEmpPhase;
}

void FKMBaseStat::SetTempo(double newTempo, bool bBroadcast)
{
	if (bBroadcast == true && Tempo != newTempo)
	{
		StatChange.Broadcast(EKMStatFactorType::Tempo, Tempo, newTempo);
	}
	Tempo = newTempo;
}

void FKMBaseStat::SetTempoRps(double newTempoRps, bool bBroadcast)
{
	if (bBroadcast == true && TempoRps != newTempoRps)
	{
		StatChange.Broadcast(EKMStatFactorType::TempoRps, TempoRps, newTempoRps);
	}
	TempoRps = newTempoRps;
}

void FKMSecondaryStat::Init(const struct FKMTable_BaseStatRow* baseStatTable)
{
	FKMBaseStat::Init(baseStatTable);
	
	HpCurr = baseStatTable->Hp * baseStatTable->HpCurr;
	SpCurr = baseStatTable->Sp * baseStatTable->SpCurr;
	MpCurr = baseStatTable->Mp * baseStatTable->MpCurr;
	EmpCurr = baseStatTable->Emp * baseStatTable->EmpCurr;
	TempoCurr = baseStatTable->Tempo * baseStatTable->TempoCurr;;
	EmpPhaseCurr = baseStatTable->EmpPhaseCurr;
}

double FKMSecondaryStat::GetCooltimeWeights() const
{
	return Dex * 0.01f;
}

void FKMSecondaryStat::ApplyDamage(double damage , bool bBroadcast)
{
	SetHpCurr(HpCurr - damage, bBroadcast);
}

void FKMSecondaryStat::SetHpCurr(double newHpCurr, bool bBroadcast)
{
	newHpCurr = FMath::Clamp<double>(newHpCurr, 0.f, GetHp());
	if (bBroadcast == true && HpCurr != newHpCurr)
	{
		StatChange.Broadcast(EKMStatFactorType::HpCurr, HpCurr, newHpCurr);
	}
	HpCurr = newHpCurr;
}

void FKMSecondaryStat::SetSpCurr(double newSpCurr, bool bBroadcast)
{
	newSpCurr = FMath::Clamp<double>(newSpCurr, 0.f, GetSp());
	if (bBroadcast == true && SpCurr != newSpCurr)
	{
		StatChange.Broadcast(EKMStatFactorType::SpCurr, SpCurr, newSpCurr);
	}
	SpCurr = newSpCurr;
}

void FKMSecondaryStat::SetMpCurr(double newMpCurr, bool bBroadcast)
{
	newMpCurr = FMath::Clamp<double>(newMpCurr, 0.f, GetMp());
	
	if (bBroadcast == true && MpCurr != newMpCurr)
	{
		StatChange.Broadcast(EKMStatFactorType::MpCurr, MpCurr, newMpCurr);
	}
	MpCurr = newMpCurr;
}

void FKMSecondaryStat::SetEmpCurr(double newEmpCurr, bool bBroadcast)
{
	if (newEmpCurr > GetEmp())
	{
		SetEmpPhaseCurr(EmpPhaseCurr + 1, bBroadcast);
		if (EmpPhaseCurr < GetEmpPhase())
		{
			newEmpCurr = FMath::Fmod(newEmpCurr, GetEmp());
		}
	}
	
	if (EmpPhaseCurr >= GetEmpPhase())
	{
		newEmpCurr = GetEmp();
	}
	
	newEmpCurr = FMath::Clamp<double>(newEmpCurr, 0.f, GetEmp());
	
	if (bBroadcast == true && EmpCurr != newEmpCurr)
	{
		StatChange.Broadcast(EKMStatFactorType::EmpCurr, EmpCurr, newEmpCurr);
	}
	EmpCurr = newEmpCurr;
}

void FKMSecondaryStat::SetEmpPhaseCurr(int32 newEmpPhaseCurr, bool bBroadcast)
{
	newEmpPhaseCurr = FMath::Clamp<int32>(newEmpPhaseCurr, 0, GetEmpPhase());

	if (bBroadcast == true && EmpPhaseCurr != newEmpPhaseCurr)
	{
		StatChange.Broadcast(EKMStatFactorType::EmpPhaseCurr, static_cast<double>(EmpPhaseCurr), static_cast<double>(newEmpPhaseCurr));
	}
	EmpPhaseCurr = newEmpPhaseCurr;
}

void FKMSecondaryStat::SetTempoCurr(double newTempoCurr, bool bBroadcast)
{
	newTempoCurr = FMath::Clamp<double>(newTempoCurr, 0.f, GetTempo());
	
	if (bBroadcast == true && TempoCurr != newTempoCurr)
	{
		StatChange.Broadcast(EKMStatFactorType::TempoCurr, TempoCurr, newTempoCurr);
	}
	TempoCurr = newTempoCurr;
}

void FKMSecondaryStat::BroadcastDamage(double damage)
{
	StatChange.Broadcast(EKMStatFactorType::Damage, 0.f, damage);	
}

void FKMSecondaryStat::BroadcastCriDamage(double damage)
{
	StatChange.Broadcast(EKMStatFactorType::DamageCri, 0.f, damage);
}

void FKMSecondaryStat::BroadcastDotDamage(double damage)
{
	StatChange.Broadcast(EKMStatFactorType::DamageDot, 0.f, damage);
}

void FKMSecondaryStat::Sanitization()
{
	HpCurr = FMath::Min(HpCurr, Hp);
	SpCurr = FMath::Min(SpCurr, Sp);
	MpCurr = FMath::Min(MpCurr, Mp);
	EmpCurr = FMath::Min(EmpCurr, Emp);
	TempoCurr = FMath::Min(TempoCurr, Tempo);
	EmpPhaseCurr = FMath::Min(EmpPhaseCurr, EmpPhase);
}

void FKMSecondaryStat::DuplicateValue(const FKMBaseStat* otherStat)
{
	FKMBaseStat::DuplicateValue(otherStat);
	
	if (otherStat->GetType() != FKMSecondaryStat::GetTypeName())
	{
		return;
	}
	
	const FKMSecondaryStat* otherSecondaryStat = static_cast<const FKMSecondaryStat*>(otherStat); 
	HpCurr = otherSecondaryStat->HpCurr;
	SpCurr = otherSecondaryStat->SpCurr;
	MpCurr = otherSecondaryStat->MpCurr;
	EmpCurr = otherSecondaryStat->EmpCurr;
	TempoCurr = otherSecondaryStat->TempoCurr;
	EmpPhaseCurr = otherSecondaryStat->EmpPhaseCurr;
}