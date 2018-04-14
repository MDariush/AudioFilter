/*
main.cpp
Created by Martin Dariush Hansen, 2018-13-04

External dependencies used in this project:
- "AudioFile" (AudioFile.cpp; AudioFile.h). For reading and writing audio files https://github.com/adamstark/AudioFile
*/

#include "AudioFile.h"
#include <iostream>
using namespace std;

// Normalize from -1..1 to 0..1
double normalize(double volume) {
	return (volume + 1.0) * 0.5;
}

int main() {

	const string fileName = "TestAudio01";

	// Load audio
	cout << "Loading audio..." << endl;

	AudioFile<double> orignal;
	orignal.load(fileName + ".wav");
	int sampleRate = orignal.getSampleRate();
	int bitDepth = orignal.getBitDepth();
	int numSamples = orignal.getNumSamplesPerChannel();
	double lengthInSeconds = orignal.getLengthInSeconds();
	int numChannels = orignal.getNumChannels();
	bool isMono = orignal.isMono();
	bool isStereo = orignal.isStereo();

	orignal.printSummary();
	cout << "Audio loaded" << endl;

	// Create and save new audio file
	cout << "Creating new audio..." << endl;

	double originalL = 0.0;
	double originalR = 0.0;
	double originalPrevL = 0.0;
	double originalPrevR = 0.0;
	double originalGradL = 0.0;
	double originalGradR = 0.0;
	double plus = 0.0;
	double plusPrev = 0.0;
	double plusGrad = 0.0;
	double minusL = 0.0;
	double minusR = 0.0;
	double minusPrevL = 0.0;
	double minusPrevR = 0.0;
	double minusGradL = 0.0;
	double minusGradR = 0.0;

	AudioFile<double> filtered = orignal;
	filtered.samples[0][0] = 0.0;
	filtered.samples[1][0] = 0.0;

	//double originalGradLNorm;
	//double originalGradRNorm;
	//double contributionL;
	//double contributionR;
	double minusMatchL = 0.0;
	double minusMatchR = 0.0;
	for (int i = 1; i < numSamples; i++) {

		originalPrevL = originalL;
		originalPrevR = originalR;
		originalL = orignal.samples[0][i] * 0.125;
		originalR = orignal.samples[1][i] * 0.125;
		originalGradL = originalL - originalPrevL;
		originalGradR = originalR - originalPrevR;
		plusPrev = plus;
		plus = (originalL + originalR) * 0.5; // @TODO: * 0.5?
		plusGrad = plus - plusPrev;
		minusPrevL = minusL;
		minusPrevR = minusR;
		minusL = (originalL - originalR) * 0.5; // @TODO: * 0.5?
		minusR = (originalR - originalL) * 0.5; // @TODO: * 0.5?
		minusGradL = minusL - minusPrevL;
		minusGradR = minusR - minusPrevR;
		

		//originalGradLNorm = normalize(originalGradL);
		//originalGradRNorm = normalize(originalGradR);
		//contributionL = originalGradLNorm / (originalGradLNorm - originalGradRNorm);
		//contributionR = originalGradRNorm / (originalGradRNorm - originalGradLNorm);
		//minusMatchL = 1.0 - abs(normalize(minusGradL) - normalize(originalGradL)); // @TODO: 0 = minusGradL matches originalGradR better than originalGradL; 1 = opposite // !!!@TODO: Shouldn't the value go above 1 to compensate the minus of R???
		//minusMatchR = 1.0 - abs(normalize(minusGradR) - normalize(originalGradL));
		//filtered.samples[0][i] = filtered.samples[0][i - 1] + minusGradL * contributionL;
		//filtered.samples[1][i] = filtered.samples[1][i - 1] + minusGradR * contributionR;
		//filtered.samples[0][i] = orignal.samples[0][i] + (orignal.samples[0][i] - orignal.samples[1][i]);
		//filtered.samples[1][i] = orignal.samples[1][i] + (orignal.samples[1][i] - orignal.samples[0][i]);
		//filtered.samples[0][i] = filtered.samples[0][i - 1] + minusGradL * minusMatchL + originalGradL * (1.0 - minusMatchL); // match: 0..1, 0 = minusGradL matches -R, 1 = minusGradL matches L
		//filtered.samples[1][i] = filtered.samples[1][i - 1] + minusGradR * minusMatchR + originalGradR * (1.0 - minusMatchR);
		//filtered.samples[0][i] = filtered.samples[0][i - 1] + minusGradL * minusMatchL;
		//filtered.samples[1][i] = filtered.samples[1][i - 1] + minusGradR * minusMatchR;



		////////////////////////////////////////
		// matchL = 1 if originalGradL is more similar to minusGradL than minusGradR
		/*double minusGradLDiffOriginalGradL = 0.0;
		double minusGradLDiffOriginalGradR = 0.0;
		double minusGradLMatchOriginalGradL = 0.0;

		minusGradLDiffOriginalGradL = minusGradL - originalGradL;
		minusGradLDiffOriginalGradR = minusGradL - originalGradR;
		minusGradLMatchOriginalGradL = minusGradLDiffOriginalGradL + minusGradLDiffOriginalGradR;

		filtered.samples[0][i] = filtered.samples[0][i - 1] + minusGradL * minusMatchL;*/



		////////////////////////////////////////
		// 0) Use grad(L+R) as gradient to construct the filtered version
		// 1) If grad(L+R) matches grad(L-R) as much as grad(R-L) then use grad(L+R)
		// 2) If grad(L+R) matches grad(L-R) better than grad(R-L) then use grad(L+R) + grad(R-L)
		// 3) If grad(L+R) matches grad(R-L) better than grad(L-R) then use grad(L+R) + grad(L-R)

		/*
		filteredPrevC = filteredC;
		double plusGradDiffMinusLGrad = 0.0;
		double plusGradDiffMinusRGrad = 0.0;
		double totalDiff = 0.0;
		double plusGradMatchesMinusLGrad = 0.0;
		double plusGradMatchesMinusRGrad = 0.0;
		double filteredC = 0.0;
		double filteredPrevC = 0.0;

		if (minusGradL != 0.0) {
			plusGradDiffMinusLGrad = minusGradL - plusGrad;
			plusGradDiffMinusRGrad = minusGradR - plusGrad;
			totalDiff = abs(plusGradDiffMinusLGrad) + abs(plusGradDiffMinusRGrad);
			plusGradMatchesMinusLGrad = abs(plusGradDiffMinusRGrad) / totalDiff;
			plusGradMatchesMinusRGrad = 1.0 - plusGradMatchesMinusLGrad;
		}
		else {
			plusGradMatchesMinusLGrad = 0.5;
			plusGradMatchesMinusRGrad = 0.5;
		}

		filteredC = filteredPrevC + plusGrad - minusGradL * plusGradMatchesMinusLGrad - minusGradR * plusGradMatchesMinusRGrad;
		if (filteredC < -1.0) {
			filteredC = -1.0;
		}
		else if (filteredC > 1.0)  {
			filteredC = 1.0;
		}
		filtered.samples[0][i] = filteredC;
		filtered.samples[1][i] = filteredC;*/



		////////////////////////////////////////
		// filteredL = minusL * match(plus, minusL), filteredR = minusR * match(plus, minusR)
		double plusDiffMinusL = 0.0;
		double plusDiffMinusR = 0.0;
		double totalDiff = 0.0;
		double plusMatchesMinusL = 0.0;
		double plusMatchesMinusR = 0.0;
		double filteredL = 0.0;
		double filteredR = 0.0;

		plusDiffMinusL = minusL - plus;
		plusDiffMinusR = minusR - plus;

		totalDiff = abs(plusDiffMinusL) + abs(plusDiffMinusR);

		plusMatchesMinusL = abs(plusDiffMinusR) / totalDiff;
		plusMatchesMinusR = abs(plusDiffMinusL) / totalDiff;

		filteredL = minusL * plusMatchesMinusL;
		filteredR = minusR * plusMatchesMinusR;

		filtered.samples[0][i] = filteredL;
		filtered.samples[1][i] = filteredR;



		////////////////////////////////////////
		// filteredL = minusGradL * match(plusGrad, minusGradL), filteredR = minusGradR * match(plusGrad, minusGradR)



		////////////////////////////////////////
		// filteredC = plus - (minusL - match?(minusR, originalR))



		///////////////////////////////////////
		// filteredC = plusGrad - (match?(minusGradL + originalGradL) + match?(minusGradR + originalGradR))



		///////////////////////////////////////
		// filteredC = plus - minusL * match?(minusGradL + originalGradL) * !match?(minusGradR + originalGradR) - minusR * match?(minusGradR + originalGradR) * !match?(minusGradL + originalGradL)
		


		///////////////////////////////////////
		// filteredL = (minusGradL + originalGradL) * match?(minusGradL, originalGradL)[0..1] * (1.0 - match?(minusGradR, originalGradR)[0..1])



		///////////////////////////////////////
		// filteredL = originalGradL * match(originalGradR + minusGradR, originalGradL + minusGradL)[0..1]
		// filteredC = plusGrad - filteredL - filteredR
	}

	filtered.save(fileName + "Filtered.wav", AudioFileFormat::Wave);

	filtered.printSummary();
	cout << "Audio created" << endl;

	// Program finish
	cout << "Program has ended" << endl;
	std::getchar();
	return 0;
}
