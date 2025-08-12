/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.
 */
#pragma once
#ifndef _FLAMENCO_FLAMENCO_H_
#define _FLAMENCO_FLAMENCO_H_

// Ѕазовые классы и объ€влени€.
#include <flamenco/core/types.h>
#include <flamenco/core/config.h>
#include <flamenco/core/utility.h>
#include <flamenco/core/lock.h>
#include <flamenco/core/atomic_types.h>
#include <flamenco/core/pin.h>
#include <flamenco/core/effect.h>
#include <flamenco/core/mixer.h>
#include <flamenco/core/source.h>
#include <flamenco/core/stream.h>

// »сточники звука.
#include <flamenco/input/procedural.h>
#include <flamenco/input/file.h>
#include <flamenco/input/memory.h>
#include <flamenco/input/wav.h>
#include <flamenco/input/wavpack.h>
#include <flamenco/input/ogg.h>

// Ёффекты.
#include <flamenco/effect/volumepan.h>
#include <flamenco/effect/pitch.h>

#endif // _FLAMENCO_FLAMENCO_H_
