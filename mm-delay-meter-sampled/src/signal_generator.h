/* Generates the speaker and LED signals.
 * Timer 1 controls the on/off interval. */
#ifndef SIGNAL_GENERATOR_H
#define SIGNAL_GENERATOR_H

void signalGeneratorSetup();
void signalGeneratorLEDOn();
void signalGeneratorLEDOff();
void signalGeneratorSpeakerOn();
void signalGeneratorSpeakerOff();
void signalGeneratorStatusLEDControl();

#endif /*SIGNAL_GENERATOR_H*/