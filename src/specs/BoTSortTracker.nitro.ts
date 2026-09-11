import type { HybridObject } from 'react-native-nitro-modules'
import { type Frame } from 'react-native-vision-camera'

export interface BoundingBox {
  x: number
  y: number
  width: number
  height: number
  confidence: number
  classId: number
}

export interface TrackedObject {
  id: number
  x: number
  y: number
  w: number
  h: number
  classId: number
}

export interface BoTSortTracker extends HybridObject<{
  ios: 'c++'
  android: 'c++'
}> {
  initialize(reidModelPath: string, useGpu: boolean): void
  updateWithFrame(frame: Frame, detections: BoundingBox[]): TrackedObject[]
}
