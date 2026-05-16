PPG Signal Components

Overview:

Photoplethysmography (PPG) is an optical sensing technique used to measure changes in blood volume within tissue. In this project, red and infrared light are emitted into the finger, and the reflected light is measured by an optical sensor. The resulting signal contains physiological information related to heart rate and blood oxygen saturation, along with non-physiological noise. Understanding the structure of the PPG waveform is essential for accurate signal processing and metric estimation.

DC Component:

The DC component represents the slowly varying baseline level of the PPG signal. It arises from constant absorption and reflection of light by skin, bone, tissue, and average blood volume. This component does not change with each heartbeat and is influenced by factors such as finger placement, contact pressure, and skin optical properties.

Although the DC component does not encode heart rate information, it is important for SpO₂ estimation, where it is used to normalize the pulsatile signal and reduce sensitivity to absolute light intensity.

AC Component:

The AC component is the small, time-varying portion of the PPG signal caused by periodic changes in arterial blood volume during the cardiac cycle. As the heart beats, blood volume increases and decreases, modulating the amount of light absorbed by the tissue. This produces a pulsatile waveform synchronized with the heartbeat.

Heart rate (BPM) is derived from the timing and frequency of these pulsations. Additionally, the relative amplitude of the AC component in the red and infrared channels is used in SpO₂ estimation.

Noise Sources:

PPG signals are susceptible to several sources of noise that can obscure or distort physiological information. Common noise sources include motion artifacts from finger movement, variations in contact pressure, ambient light leakage, and electronic noise from the sensor and analog front end.

These noise sources can introduce false peaks, alter waveform shape, or reduce signal quality, making signal conditioning and filtering necessary for reliable measurement.

Relevance to BPM and SpO₂ Estimation:

Accurate BPM estimation relies on isolating the AC component and detecting periodic peaks corresponding to heartbeats. SpO₂ estimation depends on the ratio of the AC and DC components across red and infrared wavelengths, which helps cancel out some intensity-related variations while preserving oxygen-dependent absorption differences.

As a result, effective separation of AC and DC components and mitigation of noise are critical steps in both heart rate and oxygen saturation estimation.