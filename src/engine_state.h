// engine_state.h
#pragma once
#include <cmath>

struct AudioAnalyzer; // forward declaration — null for now, real later

struct EngineState {
    // ── shared wave parameters ─────────────────
    float phase          = 0.f;
    float brightnessPhase= 0.f;
    float frequency      = 4.f;
    float amplitude      = 120.f;
    float speed          = 1.5f;
    float brightnessSpeed= 2.f;

    // ── beat system ────────────────────────────
    float bpm            = 120.f;
    float timeSinceKick  = 999.f;  // large so no kick fires on frame 0
    float kickStrength   = 0.f;    // 0..1, decays exponentially
    float kickDecay      = 8.f;    // higher = faster decay

    // ── derived (computed in main each frame) ──
    float effectiveAmplitude = 120.f;  // amplitude + kick contribution

    // ── future audio hook ──────────────────────
    AudioAnalyzer* audio = nullptr;   // null until wired up

    // ── update call (lives here for convenience) 
    void update(float dt) {
        phase          += speed          * dt;
        brightnessPhase+= brightnessSpeed* dt;
        timeSinceKick  += dt;

        // exponential kick decay
        kickStrength   *= std::exp(-kickDecay * dt);

        // derived
        effectiveAmplitude = amplitude + 120.f * kickStrength;
    }

    void triggerKick() {
        kickStrength   = 1.0f;
        timeSinceKick  = 0.f;
    }
};