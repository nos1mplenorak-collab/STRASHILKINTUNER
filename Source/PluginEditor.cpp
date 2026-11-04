#include "PluginEditor.h"
#include <cmath>
#include <vector>
using namespace juce;

//==============================================================================
// Мультяшная векторная графика
//==============================================================================
static void drawBlast (Graphics& g, float cx, float cy, float r, int seed, float alpha = 1.0f)
{
    const Colour cols[] = { Colour (0xffd01010), Colour (0xffff7a00), Colour (0xffffd400), Colour (0xfffff6b0) };
    const float  scl[]  = { 1.0f, 0.78f, 0.55f, 0.3f };

    for (int layer = 0; layer < 4; ++layer)
    {
        Random rnd (seed + layer * 7);
        Path p;
        const int spikes = 16;
        for (int i = 0; i < spikes * 2; ++i)
        {
            float a   = MathConstants<float>::twoPi * (float) i / (float) (spikes * 2);
            float rad = r * scl[layer] * ((i % 2 == 0) ? (0.85f + rnd.nextFloat() * 0.3f)
                                                         : (0.45f + rnd.nextFloat() * 0.1f));
            float x = cx + std::cos (a) * rad, y = cy + std::sin (a) * rad;
            if (i == 0) p.startNewSubPath (x, y); else p.lineTo (x, y);
        }
        p.closeSubPath();
        g.setColour (cols[layer].withAlpha (alpha));
        g.fillPath (p);
    }
}

static void drawWoman (Graphics& g, float cx, float feetY, float h, Colour bikini, Colour hair)
{
    const Colour skin (0xffffc9a0);
    const float headR    = h * 0.065f;
    const float headCy   = feetY - h + headR;
    const float torsoTop = headCy + headR + h * 0.025f;
    const float hipY     = feetY - h * 0.48f;
    const float len      = hipY - torsoTop;
    const float sw = h * 0.17f, ww = h * 0.10f, hw = h * 0.15f;

    // длинные волосы сзади
    g.setColour (hair);
    g.fillRoundedRectangle (cx - headR * 1.25f, headCy - headR * 1.1f, headR * 2.5f, headR * 5.0f, headR);

    // ноги
    g.setColour (skin);
    g.fillRoundedRectangle (cx - hw * 0.5f,  hipY - 4.0f, hw * 0.46f, feetY - hipY + 4.0f, h * 0.02f);
    g.fillRoundedRectangle (cx + hw * 0.04f, hipY - 4.0f, hw * 0.46f, feetY - hipY + 4.0f, h * 0.02f);

    // руки: одна поднята, другая на бедре
    Path arms;
    arms.startNewSubPath (cx - sw * 0.5f, torsoTop + 4.0f);
    arms.lineTo (cx - sw * 0.5f - h * 0.10f, torsoTop - h * 0.10f);
    arms.startNewSubPath (cx + sw * 0.5f, torsoTop + 4.0f);
    arms.lineTo (cx + sw * 0.5f + h * 0.08f, torsoTop + len * 0.5f);
    arms.lineTo (cx + hw * 0.5f, hipY - 4.0f);
    g.strokePath (arms, PathStrokeType (h * 0.032f, PathStrokeType::curved, PathStrokeType::rounded));

    // торс
    Path torso;
    torso.startNewSubPath (cx - sw / 2, torsoTop);
    torso.lineTo (cx + sw / 2, torsoTop);
    torso.lineTo (cx + ww / 2, torsoTop + len * 0.5f);
    torso.lineTo (cx + hw / 2, hipY);
    torso.lineTo (cx - hw / 2, hipY);
    torso.lineTo (cx - ww / 2, torsoTop + len * 0.5f);
    torso.closeSubPath();
    g.fillPath (torso);

    // шея и голова
    g.fillRect (cx - headR * 0.3f, headCy + headR * 0.7f, headR * 0.6f, headR * 0.8f);
    g.fillEllipse (cx - headR, headCy - headR, headR * 2, headR * 2);

    // чёлка + солнечные очки
    g.setColour (hair);
    Path fringe;
    fringe.addPieSegment (cx - headR * 1.05f, headCy - headR * 1.05f, headR * 2.1f, headR * 2.1f,
                          MathConstants<float>::pi * 1.05f, MathConstants<float>::pi * 1.95f, 0.0f);
    g.fillPath (fringe);
    g.setColour (Colours::black);
    g.fillRoundedRectangle (cx - headR * 0.85f, headCy - headR * 0.1f, headR * 0.75f, headR * 0.45f, 3.0f);
    g.fillRoundedRectangle (cx + headR * 0.10f, headCy - headR * 0.1f, headR * 0.75f, headR * 0.45f, 3.0f);
    g.setColour (Colour (0xffd03050));
    g.fillRoundedRectangle (cx - headR * 0.35f, headCy + headR * 0.5f, headR * 0.7f, headR * 0.2f, 2.0f);

    // купальник
    g.setColour (bikini);
    const float topY = torsoTop + len * 0.14f;
    g.fillEllipse (cx - sw * 0.40f, topY, sw * 0.38f, sw * 0.30f);
    g.fillEllipse (cx + sw * 0.02f, topY, sw * 0.38f, sw * 0.30f);
    g.fillRect (cx - sw * 0.40f, topY + sw * 0.02f, sw * 0.80f, sw * 0.05f);
    Path bottom;
    bottom.addTriangle (cx - hw * 0.55f, hipY - len * 0.20f, cx + hw * 0.55f, hipY - len * 0.20f, cx, hipY + len * 0.12f);
    g.fillPath (bottom);
}

static void drawTruck (Graphics& g, float cx, float groundY, float s, Colour body)
{
    const float wr = 34.0f * s;
    const float wy = groundY - wr;
    const float wx1 = cx - 78.0f * s, wx2 = cx + 78.0f * s;
    const float bodyBottom = groundY - 2.0f * wr - 6.0f * s;
    const float bodyTop = bodyBottom - 38.0f * s;

    // подвеска
    g.setColour (Colours::grey.darker());
    g.drawLine (wx1, wy, wx1, bodyBottom, 5.0f * s);
    g.drawLine (wx2, wy, wx2, bodyBottom, 5.0f * s);

    // кузов + кабина
    g.setColour (body);
    g.fillRoundedRectangle (cx - 120.0f * s, bodyTop, 240.0f * s, 38.0f * s, 8.0f * s);
    g.fillRoundedRectangle (cx - 20.0f * s, bodyTop - 32.0f * s, 100.0f * s, 38.0f * s, 10.0f * s);
    g.setColour (Colour (0xffbfe9ff));
    g.fillRoundedRectangle (cx - 10.0f * s, bodyTop - 26.0f * s, 80.0f * s, 22.0f * s, 5.0f * s);
    // полоса-пламя
    g.setColour (Colour (0xffffd400));
    Path flame;
    flame.startNewSubPath (cx - 115.0f * s, bodyTop + 26.0f * s);
    flame.lineTo (cx - 60.0f * s, bodyTop + 14.0f * s);
    flame.lineTo (cx - 20.0f * s, bodyTop + 24.0f * s);
    flame.lineTo (cx + 40.0f * s, bodyTop + 10.0f * s);
    flame.lineTo (cx + 110.0f * s, bodyTop + 28.0f * s);
    flame.lineTo (cx + 110.0f * s, bodyTop + 34.0f * s);
    flame.lineTo (cx - 115.0f * s, bodyTop + 34.0f * s);
    flame.closeSubPath();
    g.fillPath (flame);

    // колёса
    for (float wx : { wx1, wx2 })
    {
        g.setColour (Colour (0xff111111));
        g.fillEllipse (wx - wr, wy - wr, wr * 2, wr * 2);
        g.setColour (Colour (0xff333333));
        g.drawEllipse (wx - wr * 0.8f, wy - wr * 0.8f, wr * 1.6f, wr * 1.6f, 3.0f * s);
        g.setColour (Colour (0xffc0c0c0));
        g.fillEllipse (wx - wr * 0.42f, wy - wr * 0.42f, wr * 0.84f, wr * 0.84f);
        g.setColour (Colour (0xff555555));
        g.fillEllipse (wx - wr * 0.12f, wy - wr * 0.12f, wr * 0.24f, wr * 0.24f);
    }
}

//==============================================================================
TunerAudioProcessorEditor::TunerAudioProcessorEditor (TunerAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setSize (720, 480);

    a4Slider.setSliderStyle (Slider::LinearHorizontal);
    a4Slider.setTextBoxStyle (Slider::TextBoxLeft, false, 80, 22);
    a4Slider.setTextValueSuffix (" Hz  (A4)");
    a4Slider.setColour (Slider::thumbColourId, Colours::yellow);
    a4Slider.setColour (Slider::trackColourId, Colours::orange);
    a4Slider.setColour (Slider::textBoxTextColourId, Colours::white);
    a4Slider.setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible (a4Slider);
    a4Attachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, "a4", a4Slider);

    startTimerHz (20);
}

void TunerAudioProcessorEditor::resized()
{
    a4Slider.setBounds (205, 300, 310, 28);
}

//==============================================================================
// Определение высоты тона (алгоритм YIN)
//==============================================================================
float TunerAudioProcessorEditor::detectPitch()
{
    constexpr int N = 4096, W = 2048;
    const double sr = proc.sampleRate.load();

    std::vector<float> x ((size_t) N);
    const int wp = proc.writePos.load (std::memory_order_acquire);
    double energy = 0.0;
    for (int i = 0; i < N; ++i)
    {
        float v = proc.ring[(size_t) ((wp - N + i) & TunerAudioProcessor::ringMask)].load (std::memory_order_relaxed);
        x[(size_t) i] = v;
        if (i < W) energy += (double) v * v;
    }
    if (std::sqrt (energy / W) < 0.004) return -1.0f;   // тишина

    const int tauMax = jmin ((int) (sr / 40.0), N - W - 1);
    const int tauMin = jmax (2, (int) (sr / 2000.0));
    if (tauMax <= tauMin + 2) return -1.0f;

    std::vector<float> d ((size_t) tauMax + 1, 0.0f), cm ((size_t) tauMax + 1, 1.0f);
    for (int tau = 1; tau <= tauMax; ++tau)
    {
        float sum = 0.0f;
        for (int j = 0; j < W; ++j)
        {
            float diff = x[(size_t) j] - x[(size_t) (j + tau)];
            sum += diff * diff;
        }
        d[(size_t) tau] = sum;
    }
    float running = 0.0f;
    for (int tau = 1; tau <= tauMax; ++tau)
    {
        running += d[(size_t) tau];
        cm[(size_t) tau] = running > 0.0f ? d[(size_t) tau] * (float) tau / running : 1.0f;
    }

    int best = -1;
    for (int tau = tauMin; tau < tauMax; ++tau)
    {
        if (cm[(size_t) tau] < 0.15f)
        {
            while (tau + 1 < tauMax && cm[(size_t) tau + 1] < cm[(size_t) tau]) ++tau;
            best = tau;
            break;
        }
    }
    if (best < 0) return -1.0f;

    float t = (float) best;
    if (best > 1 && best < tauMax)
    {
        float a = cm[(size_t) best - 1], b = cm[(size_t) best], c = cm[(size_t) best + 1];
        float den = a - 2.0f * b + c;
        if (std::abs (den) > 1.0e-9f) t += 0.5f * (a - c) / den;
    }
    return (float) (sr / t);
}

void TunerAudioProcessorEditor::timerCallback()
{
    const float f = detectPitch();
    if (f > 0.0f)
    {
        freq = (freq > 0.0f) ? 0.6f * freq + 0.4f * f : f;
        missed = 0;
    }
    else if (++missed > 6)
        freq = -1.0f;

    active = freq > 0.0f;
    if (active)
    {
        const float a4 = *proc.apvts.getRawParameterValue ("a4");
        const float midi = 69.0f + 12.0f * std::log2 (freq / a4);
        const int n = (int) std::lround (midi);
        cents = 100.0f * (midi - (float) n);
        static const char* names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        noteName = String (names[((n % 12) + 12) % 12]) + String (n / 12 - 1);
    }
    else
    {
        noteName = "--";
        cents = 0.0f;
    }
    repaint();
}

//==============================================================================
void TunerAudioProcessorEditor::paint (Graphics& g)
{
    const auto b = getLocalBounds().toFloat();

    // небо-закат
    g.setGradientFill (ColourGradient (Colour (0xff12002e), 0, 0, Colour (0xffff5a00), 0, b.getHeight(), false));
    g.fillAll();

    // земля
    g.setColour (Colour (0xff3a2210));
    g.fillRect (0.0f, 452.0f, b.getWidth(), 28.0f);

    // взрывы
    drawBlast (g, 360, 200, 230, 11, 0.55f);
    drawBlast (g, 60, 70, 70, 3);
    drawBlast (g, 665, 80, 80, 5);
    drawBlast (g, 120, 410, 50, 8);
    drawBlast (g, 610, 405, 55, 9);

    // монстр-траки
    drawTruck (g, 150, 452, 0.9f, Colour (0xff2fbf3a));
    drawTruck (g, 575, 452, 0.9f, Colour (0xffe02020));
    drawTruck (g, 360, 452, 0.75f, Colour (0xff2060ff));

    // девушки в купальниках
    drawWoman (g, 70, 448, 290, Colour (0xffff2d95), Colour (0xff3a1a0a));
    drawWoman (g, 650, 448, 290, Colour (0xff00c8ff), Colour (0xfff2d060));

    // заголовок
    g.setFont (Font (40.0f, Font::bold));
    const String title = String (CharPointer_UTF8 ("\xD0\xA8\xD0\xA2\xD0\xA0\xD0\x90\xD0\xA8\xD0\x98\xD0\x9B\xD0\x9A\xD0\x98\xD0\x9D \xD0\xA2\xD0\xAE\xD0\x9D\xD0\x95\xD0\xA0"));
    Rectangle<int> tr (0, 8, getWidth(), 56);
    g.setColour (Colours::black);
    for (int dx = -3; dx <= 3; dx += 3)
        for (int dy = -3; dy <= 3; dy += 3)
            g.drawText (title, tr.translated (dx, dy), Justification::centred);
    g.setColour (Colour (0xffffe600));
    g.drawText (title, tr, Justification::centred);

    // панель тюнера
    Rectangle<float> panel (190.0f, 78.0f, 340.0f, 258.0f);
    g.setColour (Colours::black.withAlpha (0.72f));
    g.fillRoundedRectangle (panel, 16.0f);
    g.setColour (Colour (0xffffe600));
    g.drawRoundedRectangle (panel, 16.0f, 3.0f);

    const bool inTune = active && std::abs (cents) < 5.0f;
    const Colour accent = ! active ? Colours::grey : (inTune ? Colour (0xff30ff60) : Colour (0xffff5030));

    g.setColour (accent);
    g.setFont (Font (84.0f, Font::bold));
    g.drawText (noteName, Rectangle<int> (190, 84, 340, 96), Justification::centred);

    g.setColour (Colours::white);
    g.setFont (Font (20.0f));
    g.drawText (active ? String (freq, 1) + " Hz   " + (cents >= 0 ? "+" : "") + String (cents, 0) + " cents"
                       : String ("play a note"),
                Rectangle<int> (190, 180, 340, 28), Justification::centred);

    // шкала центов
    const float bx = panel.getX() + 25.0f, bw = panel.getWidth() - 50.0f, by = 238.0f;
    g.setColour (Colours::white.withAlpha (0.25f));
    g.fillRoundedRectangle (bx, by, bw, 14.0f, 7.0f);
    g.setColour (Colour (0xff30ff60).withAlpha (0.6f));
    g.fillRect (bx + bw * 0.45f, by, bw * 0.10f, 14.0f);
    g.setColour (Colours::white);
    for (int i = -5; i <= 5; ++i)
        g.drawLine (bx + bw * (0.5f + 0.1f * (float) i), by - 6.0f, bx + bw * (0.5f + 0.1f * (float) i),
                    by + (i == 0 ? 24.0f : 18.0f), i == 0 ? 3.0f : 1.5f);

    if (active)
    {
        const float nx = bx + bw * (0.5f + jlimit (-50.0f, 50.0f, cents) / 100.0f);
        Path needle;
        needle.addTriangle (nx, by - 8.0f, nx - 9.0f, by - 26.0f, nx + 9.0f, by - 26.0f);
        g.setColour (accent);
        g.fillPath (needle);
        g.drawLine (nx, by - 8.0f, nx, by + 22.0f, 3.0f);
    }
}
