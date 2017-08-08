/*
  ==============================================================================

    FeatureAnalysis.cpp
    Created: 4 Aug 2017 9:53:11am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "FeatureAnalysis.h"


FeatureAnalysis::FeatureAnalysis(const DBConnector& db) : db_(db)
{
    sampleRate_ = 44100;
    frameSize_ = 2048;
    hopSize_ = 256;
    
    essentia::init();
    AlgorithmFactory& factory = AlgorithmFactory::instance();
    
    audio_ = factory.create("MonoLoader",
                            "sampleRate", sampleRate_);
    
    audioEq_ = factory.create("EqloudLoader",
                              "sampleRate", sampleRate_);
    
    spectral_ = factory.create("LowLevelSpectralExtractor",
                               "frameSize", frameSize_,
                               "hopSize", hopSize_,
                               "sampleRate", sampleRate_);
    
    spectralEq_ = factory.create("LowLevelSpectralEqloudExtractor",
                                 "frameSize", frameSize_,
                                 "hopSize", hopSize_,
                                 "sampleRate", sampleRate_);
    
    frameCutter_ = factory.create("FrameCutter",
                                  "frameSize", 64,
                                  "hopSize", 32);
    
    startStopSilence_ = factory.create("StartStopSilence");
    
    std::vector<std::string> stats = {"mean", "stdev"};
    aggr_ = factory.create("PoolAggregator",
                           "defaultStats", stats);
    
    envelope_ = factory.create("Envelope");
    
    lat_ = factory.create("LogAttackTime");
    
    centroid_ = factory.create("Centroid");
    
    trimmer_ = factory.create("Trimmer");
    
    rms_ = factory.create("RMS");
    
}


FeatureAnalysis::~FeatureAnalysis()
{
    delete audio_;
    delete audioEq_;
    delete spectral_;
    delete spectralEq_;
    delete aggr_;
    delete frameCutter_;
    delete startStopSilence_;
    delete envelope_;
    delete lat_;
    delete centroid_;
    delete trimmer_;
    delete rms_;
    
    essentia::shutdown();
}


void FeatureAnalysis::run(Sample::Ptr sample, double segStart, double segLength)
{
    Pool pool;
    
    // Configure the audio loaders with new sample
    audio_->reset();
    audioEq_->reset();
    spectral_->reset();
    spectralEq_->reset();
    aggr_->reset();
    frameCutter_->reset();
    startStopSilence_->reset();
    envelope_->reset();
    centroid_->reset();
    trimmer_->reset();
    rms_->reset();
    
    audio_->configure("filename", sample->getFile().getFullPathName().toStdString());
    audioEq_->configure("filename", sample->getFile().getFullPathName().toStdString());
    
    // Set up and read audio samples from file
    std::vector<Real> audioBuffer;
    std::vector<Real> audioEqBuffer;
    
    audio_->output("audio").set(audioBuffer);
    audioEq_->output("audio").set(audioEqBuffer);
    
    audio_->compute();
    audioEq_->compute();
    
    // Onset Detection
    std::vector<Real> frame;
    int startFrame;
    int stopFrame;
    
    frameCutter_->input("signal").set(audioBuffer);
    frameCutter_->output("frame").set(frame);
    startStopSilence_->input("frame").set(frame);
    startStopSilence_->output("startFrame").set(startFrame);
    startStopSilence_->output("stopFrame").set(stopFrame);
    
    while(true)
    {
        frameCutter_->compute();
        if (!frame.size())
        {
            break;
        }
        startStopSilence_->compute();
    }
    
    // Save the detected onset and end time for the sample
    sample->setStartTime(double(startFrame * 32) / sampleRate_);
    sample->setStopTime(double(stopFrame * 32) / sampleRate_);
    
    // Compute LAT and temporal centroid
    std::vector<Real> envelopeBuffer;
    Real lat;
    Real attackStart;
    Real attackStop;
    Real tc;
    
    envelope_->input("signal").set(audioEqBuffer);
    envelope_->output("signal").set(envelopeBuffer);
    lat_->input("signal").set(envelopeBuffer);
    lat_->output("logAttackTime").set(lat);
    lat_->output("attackStart").set(attackStart);
    lat_->output("attackStop").set(attackStop);
    
    centroid_->configure("range", Real((audioEqBuffer.size() - 1)) / sampleRate_);
    centroid_->input("array").set(envelopeBuffer);
    centroid_->output("centroid").set(tc);
    
    envelope_->compute();
    lat_->compute();
    centroid_->compute();
    
    // Apply time segmentation
    Real windowStart;
    Real windowEnd;
    Real latUnused;
    
    lat_->reset();
    lat_->configure("startAttackThreshold", segStart > 0.9 ? 0.9 : Real(segStart));
    lat_->input("signal").set(envelopeBuffer);
    lat_->output("logAttackTime").set(latUnused);
    lat_->output("attackStart").set(windowStart);
    lat_->output("attackStop").set(windowEnd);
    
    lat_->compute();
    windowStart = segStart > 0.9 ? windowEnd : windowStart;
    
    if (segLength > 0)
    {
        std::vector<Real> trimBuffer;
        
        trimmer_->configure("startTime", windowStart);
        trimmer_->configure("endTime", windowStart + segLength);
        
        trimmer_->input("signal").set(audioBuffer);
        trimmer_->output("signal").set(trimBuffer);
        trimmer_->compute();
        
        audioBuffer = trimBuffer;
        
        // Now compute for the equal loudness audio input
        trimmer_->input("signal").set(audioEqBuffer);
        trimmer_->compute();
        
        audioEqBuffer = trimBuffer;
    }
    
    // Calculat RMS on trimmed sample
    Real rms;
    rms_->input("array").set(audioEqBuffer);
    rms_->output("rms").set(rms);
    rms_->compute();
    rms = 20*log10(rms);
    
    // Collecting variables for Eqloud spectral extractor output
    std::vector<std::vector<Real>> barkbands;
    std::vector<Real> barkbands_kurtosis;
    std::vector<Real> barkbands_skewness;
    std::vector<Real> barkbands_spread;
    std::vector<Real> hfc;
    std::vector<std::vector<Real>> mfcc;
    std::vector<Real> pitch;
    std::vector<Real> pitch_instantaneous_confidence;
    std::vector<Real> pitch_salience;
    std::vector<Real> silence_rate_20dB;
    std::vector<Real> silence_rate_30dB;
    std::vector<Real> silence_rate_60dB;
    std::vector<Real> spectral_complexity;
    std::vector<Real> spectral_crest;
    std::vector<Real> spectral_decrease;
    std::vector<Real> spectral_energy;
    std::vector<Real> spectral_energyband_low;
    std::vector<Real> spectral_energyband_middle_low;
    std::vector<Real> spectral_energyband_middle_high;
    std::vector<Real> spectral_energyband_high;
    std::vector<Real> spectral_flatness_db;
    std::vector<Real> spectral_flux;
    std::vector<Real> spectral_rms;
    std::vector<Real> spectral_rolloff;
    std::vector<Real> spectral_strongpeak;
    std::vector<Real> zerocrossingrate;
    std::vector<Real> inharmonicity;
    std::vector<std::vector<Real>> tristimulus;
    std::vector<Real> oddtoevenharmonicenergyratio;
    
    // --- Connections for Eqloud spectral extractor
    spectral_->input("signal").set(audioBuffer);
    spectral_->output("barkbands").set(barkbands);
    spectral_->output("barkbands_skewness").set(barkbands_skewness);
    spectral_->output("barkbands_kurtosis").set(barkbands_kurtosis);
    spectral_->output("barkbands_spread").set(barkbands_spread);
    spectral_->output("hfc").set(hfc);
    spectral_->output("mfcc").set(mfcc);
    spectral_->output("pitch").set(pitch);
    spectral_->output("pitch_instantaneous_confidence").set(pitch_instantaneous_confidence);
    spectral_->output("pitch_salience").set(pitch_salience);
    spectral_->output("silence_rate_20dB").set(silence_rate_20dB);
    spectral_->output("silence_rate_30dB").set(silence_rate_30dB);
    spectral_->output("silence_rate_60dB").set(silence_rate_60dB);
    spectral_->output("spectral_complexity").set(spectral_complexity);
    spectral_->output("spectral_crest").set(spectral_crest);
    spectral_->output("spectral_decrease").set(spectral_decrease);
    spectral_->output("spectral_energy").set(spectral_energy);
    spectral_->output("spectral_energyband_low").set(spectral_energyband_low);
    spectral_->output("spectral_energyband_middle_low").set(spectral_energyband_middle_low);
    spectral_->output("spectral_energyband_middle_high").set(spectral_energyband_middle_high);
    spectral_->output("spectral_energyband_high").set(spectral_energyband_high);
    spectral_->output("spectral_flatness_db").set(spectral_flatness_db);
    spectral_->output("spectral_flux").set(spectral_flux);
    spectral_->output("spectral_rms").set(spectral_rms);
    spectral_->output("spectral_rolloff").set(spectral_rolloff);
    spectral_->output("spectral_strongpeak").set(spectral_strongpeak);
    spectral_->output("zerocrossingrate").set(zerocrossingrate);
    spectral_->output("inharmonicity").set(inharmonicity);
    spectral_->output("tristimulus").set(tristimulus);
    spectral_->output("oddtoevenharmonicenergyratio").set(oddtoevenharmonicenergyratio);
    
    // Compute low level spectral indicators
    spectral_->compute();
    
    // Collection variables for output of spectral extractor
    std::vector<Real> dissonance;
    std::vector<std::vector<Real>> sccoeffs;
    std::vector<std::vector<Real>> scvalleys;
    std::vector<Real> spectral_centroid;
    std::vector<Real> spectral_kurtosis;
    std::vector<Real> spectral_skewness;
    std::vector<Real> spectral_spread;
    
    // Connections for spectral extractor
    spectralEq_->input("signal").set(audioEqBuffer);
    spectralEq_->output("dissonance").set(dissonance);
    spectralEq_->output("sccoeffs").set(sccoeffs);
    spectralEq_->output("scvalleys").set(scvalleys);
    spectralEq_->output("spectral_centroid").set(spectral_centroid);
    spectralEq_->output("spectral_kurtosis").set(spectral_kurtosis);
    spectralEq_->output("spectral_skewness").set(spectral_skewness);
    spectralEq_->output("spectral_spread").set(spectral_spread);

    // Compute low level spectral indicators with equal loudness filter
    spectralEq_->compute();
    
    pool.merge("barkbands", barkbands, "replace");
    pool.add("barkbands_kurtosis", barkbands_kurtosis);
    pool.add("barkbands_skewness", barkbands_skewness);
    pool.add("barkbands_spread", barkbands_spread);
    pool.add("hfc", hfc);
    pool.merge("mfcc", mfcc, "replace");
    pool.add("pitch", pitch);
    pool.add("pitch_instantaneous_confidence", pitch_instantaneous_confidence);
    pool.add("pitch_salience", pitch_salience);
    pool.add("silence_rate_20dB", silence_rate_20dB);
    pool.add("silence_rate_30dB", silence_rate_30dB);
    pool.add("silence_rate_60dB", silence_rate_60dB);
    pool.add("spectral_complexity", spectral_complexity);
    pool.add("spectral_crest", spectral_crest);
    pool.add("spectral_decrease", spectral_decrease);
    pool.add("spectral_energy", spectral_energy);
    pool.add("spectral_energyband_low", spectral_energyband_low);
    pool.add("spectral_energyband_middle_low", spectral_energyband_middle_low);
    pool.add("spectral_energyband_middle_high", spectral_energyband_middle_high);
    pool.add("spectral_energyband_high", spectral_energyband_high);
    pool.add("spectral_flatness_db", spectral_flatness_db);
    pool.add("spectral_flux", spectral_flux);
    pool.add("spectral_rms", spectral_rms);
    pool.add("spectral_rolloff", spectral_rolloff);
    pool.add("spectral_strongpeak", spectral_strongpeak);
    pool.add("inharmonicity", inharmonicity);
    pool.merge("tristimulus", tristimulus, "replace");
    pool.add("oddtoevenharmonicenergyratio", oddtoevenharmonicenergyratio);
    pool.add("dissonance", dissonance);
    pool.merge("sccoeffs", sccoeffs, "replace");
    pool.merge("scvalleys", scvalleys, "replace");
    pool.add("spectral_centroid", spectral_centroid);
    pool.add("spectral_kurtosis", spectral_kurtosis);
    pool.add("spectral_skewness", spectral_skewness);
    pool.add("spectral_spread", spectral_spread);
    
    // Stats aggregration
    Pool aggrPool;
    
    aggr_->input("input").set(pool);
    aggr_->output("output").set(aggrPool);
    aggr_->compute();
    
    const std::map<std::string, std::vector<Real>>& vectorResults = aggrPool.getRealPool();
    const std::map<std::string, Real>& realResults = aggrPool.getSingleRealPool();
    
    std::cout << vectorResults.size() + realResults.size() << "\n";
    
}
