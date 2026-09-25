import type { HybridObject } from 'react-native-nitro-modules';
import { type Frame } from 'react-native-vision-camera';

export interface BoundingBox {
  x: number;
  y: number;
  width: number;
  height: number;
  confidence: number;
  classId: number;
}

export interface TrackedObject {
  id: number;
  x: number;
  y: number;
  w: number;
  h: number;
  classId: number;
}

export enum GMCMethod {
  ORB,
  ECC,
  SOF,
  VideoStab,
}

export interface BoTSORTConfig {
  enable_gmc?: boolean; // if true, Global Motion Compensation is enabled
  track_high_thresh?: number; // confidence threshold to classify a detection as high confidence detection. These detections are used in 1st level of association and to confirm a track
  track_low_thresh?: number; // lowest possible confidence to use a detection in the tracking algo. Any detection having confidence below this threshold is discarded
  new_track_thresh?: number; // confidence threshold to start a new track
  track_buffer?: number; // number governs the number of frames a track is kept alive without any detection. max_alive_age = frame_rate / 30.0 * track_buffer
  match_thresh?: number; // cost threshold to match a detection to a track (iou + embedding distance), only used in 1st level of association
  proximity_thresh?: number; // IoU distance (1 - IoU) threshold to reject a detection. If a detection <-> track box IoU distance is greater than this threshold, the match is rejected
  appearance_thresh?: number; // embedding distance threshold to reject a detection. If a detection <-> track embedding distance is greater than this threshold, the match is rejected
  frame_rate?: number; // frame rate of the video being processed
  gmc_method?: GMCMethod;
  lambda?: number; // factor for fusing motion (mahalanobis distance) and appearance information; fused_distance = lambda * motion_distance + (1 - lambda) * appearance_distance}
}

export interface OrbGMCConfig {
  gmc_method: GMCMethod.ORB;
  downscale?: number;
  inlier_ratio?: number;
  ransac_conf?: number;
  ransac_max_iters?: number;
}

export interface EccGMCConfig {
  gmc_method: GMCMethod.ORB;
  downscale?: number;
  max_iterations?: number;
  termination_eps?: number;
}

export interface SofGMCConfig {
  gmc_method: GMCMethod.SOF;
  downscale?: number;
  use_harris_detector?: boolean;
  max_corners?: number;
  block_size?: number;
  quality_level?: number;
  k?: number;
  min_distance?: number;
  inlier_ratio?: number;
  ransac_conf?: number;
  ransac_max_iters?: number;
}

export interface VideoStabGMCConfig {
  gmc_method: GMCMethod.VideoStab;
  downscale?: number;
  num_features?: number;
  detections_masking?: boolean;
}

export type GMCConfig =
  OrbGMCConfig | EccGMCConfig | SofGMCConfig | VideoStabGMCConfig;

export interface BoTSortTracker extends HybridObject<{
  ios: 'c++';
  android: 'c++';
}> {
  initialize(trackerConfig: BoTSORTConfig, gmcConfig?: GMCConfig): void;
  updateWithFrame(frame: Frame, detections: BoundingBox[]): TrackedObject[];
}
