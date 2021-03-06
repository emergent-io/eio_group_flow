import sys
import soundfile as sf
from math import *

# utility

def normAndClip(value, srcLow, srcHigh, dstLow=0.0, dstHigh=1.0):
    if(srcLow == srcHigh):
        return dstLow
    valueNorm = (value - srcLow) / (srcHigh - srcLow)
    valueNorm = min(max(valueNorm, 0), 1)
    return dstLow + valueNorm * (dstHigh-dstLow)

def atodb(amp):
    if(amp == 0):
        return -200
    return 20 * log(amp, 10)



# inputs and settings

inputFile = sys.argv[1]
outputFile = '02-nrm.wav'
outputInfo = 'normInfo.txt'
ignoreFirstSeconds = 0.1
truncateToLength = 630
minimumGainDb = -60
ignoreWindows = [[108, 113], [600, 630]]

def inIgnoreWindows(time):
    for window in ignoreWindows:
        if(time >= window[0] and time <= window[1]):
            return True
    return False

data,sampleRate = sf.read(inputFile)
print data.shape[0]

print 'sample rate ' + str(sampleRate)
print 'sample count ' + str(len(data))
print 'length secs. ' + str(len(data) / float(sampleRate))
print 'track count ' + str(len(data[0]))

sampleCount = len(data)
trackCount = len(data[0])

#get min and max values

minValues = [10000000]*trackCount
maxValues = [-10000000]*trackCount
minValueTimes = [0.0]*trackCount
maxValueTimes = [0.0]*trackCount

for frameIndex,frame in enumerate(data):
    frameTime = frameIndex / float(sampleRate)
    if(frameTime < ignoreFirstSeconds or frameTime > truncateToLength or inIgnoreWindows(frameTime)):
        continue
    for trackIndex in xrange(0,trackCount):
        trackValue = frame[trackIndex]
        if(trackValue < minValues[trackIndex]):
            minValues[trackIndex] = trackValue
            minValueTimes[trackIndex] = frameTime
        if(trackValue > maxValues[trackIndex]):
            maxValues[trackIndex] = trackValue
            maxValueTimes[trackIndex] = frameTime

trackDbRanges = []

normInfoFile = open(outputInfo, 'w')

for trackIndex in xrange(0,trackCount):
    dbRange = atodb(maxValues[trackIndex] - minValues[trackIndex])
    trackDbRanges.append(dbRange)
    lineStart = '{0}: {1}'.format(1+trackIndex, dbRange)
    chNormInfo = '{0},     {1:f}, {2:f},    {3:f}, {4:f}'.format(lineStart, minValues[trackIndex], maxValues[trackIndex], float(minValueTimes[trackIndex]), float(maxValueTimes[trackIndex]))
    print chNormInfo
    normInfoFile.write(chNormInfo + '\n')



# output normalized signals
outputFrame = [0.0]*trackCount
outputData = []
for frameIndex,frame in enumerate(data):
    frameTime = frameIndex / float(sampleRate)
    if(frameTime > truncateToLength):
        break
    for trackIndex in xrange(0,trackCount):
        outputValue = 0.0
        if(trackDbRanges[trackIndex] > minimumGainDb):
            outputValue = normAndClip(frame[trackIndex], minValues[trackIndex], maxValues[trackIndex], -1.0, 1.0)
        frame[trackIndex] = outputValue

# truncate data to required length
if(truncateToLength != 0.0):
    truncatedFrameCount = int(floor(sampleRate*truncateToLength))
    print 'truncating to {0} seconds, {1} sample frames'.format(truncateToLength, truncatedFrameCount)
    data = data[0:truncatedFrameCount]

sf.write(outputFile, data, sampleRate)
        
