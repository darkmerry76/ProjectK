#pragma once

#include "CoreMinimal.h"
#include "EMTableCore.h"

#define LIBXL_USERNAME	TEXT("daesung kim")
#define LIBXL_KEY		TEXT("windows-222b250607c5e40769bf6962aevfj6u6")

DECLARE_LOG_CATEGORY_EXTERN(EMLog, Log, All);

#define EM_LOG(CategoryName, Verbosity, Format,...) \
	EM_PRIVATE_LOG(PREPROCESSOR_NOTHING, constexpr, CategoryName, Verbosity, Format, ##__VA_ARGS__)

#define EM_PRIVATE_LOG(Condition, CategoryConst, Category, Verbosity, Format, ...) \
	{ \
		switch(ELogVerbosity::Verbosity)\
		{\
			case ELogVerbosity::Fatal: printf("\x1b[31m"); break;\
			case ELogVerbosity::Error: printf("\x1b[31m"); break;\
			case ELogVerbosity::Warning: printf("\x1b[33m"); break;\
			default:printf("\x1b[0m"); break;\
		}\
		static_assert(std::is_const_v<std::remove_reference_t<decltype(Format)>>, "Formatting string must be a const TCHAR array."); \
		static_assert(TIsArrayOrRefOfTypeByPredicate<decltype(Format), TIsCharEncodingCompatibleWithTCHAR>::Value, "Formatting string must be a TCHAR array."); \
		UE_VALIDATE_FORMAT_STRING(Format, ##__VA_ARGS__); \
		static ::UE::Logging::Private::FStaticBasicLogDynamicData LOG_Dynamic; \
		/* This variable can only be constexpr if the __builtin_FILE() and __builtin_LINE() intrinsic functions are constexpr - otherwise make it plain const */ \
		static PREPROCESSOR_IF(PLATFORM_COMPILER_SUPPORTS_CONSTEXPR_BUILTIN_FILE_AND_LINE, constexpr, const) ::UE::Logging::Private::FStaticBasicLogRecord LOG_Static(Format, __builtin_FILE(), __builtin_LINE(), ::ELogVerbosity::Verbosity, LOG_Dynamic); \
		static_assert((::ELogVerbosity::Verbosity & ::ELogVerbosity::VerbosityMask) < ::ELogVerbosity::NumVerbosity && ::ELogVerbosity::Verbosity > 0, "Verbosity must be constant and in range."); \
		if constexpr ((::ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) == ::ELogVerbosity::Fatal) \
		{ \
			Condition \
			{ \
				::UE::Logging::Private::BasicFatalLog(Category, &LOG_Static, ##__VA_ARGS__); \
				CA_ASSUME(false); \
			} \
		} \
		else if constexpr ((::ELogVerbosity::Verbosity & ::ELogVerbosity::VerbosityMask) <= ::ELogVerbosity::COMPILED_IN_MINIMUM_VERBOSITY) \
		{ \
			if CategoryConst ((::ELogVerbosity::Verbosity & ::ELogVerbosity::VerbosityMask) <= Category.GetCompileTimeVerbosity()) \
			{ \
				if (!Category.IsSuppressed(::ELogVerbosity::Verbosity)) \
				{ \
					Condition \
					{ \
						::UE::Logging::Private::BasicLog(Category, &LOG_Static, ##__VA_ARGS__); \
					} \
				} \
			} \
		} \
	}

#define DEFAULT_ESPMODE	ESPMode::NotThreadSafe

