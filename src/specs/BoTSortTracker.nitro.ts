import type { HybridObject } from 'react-native-nitro-modules';
import { type Frame } from 'react-native-vision-camera';

/**
 * All x, y, width, height should be in absolute values \
 * Follows the format top-left-x, top-left-y
 */
export interface BoundingBox {
  x: number;
  y: number;
  width: number;
  height: number;
  confidence: number;
  classId: number;
}

/**
 * All x, y, width, height should be in absolute values \
 * Follows the format top-left-x, top-left-y
 */
export interface TrackedObject {
  id: number;
  x: number;
  y: number;
  w: number;
  h: number;
  classId: number;
}

/**
 * GMC algorithim methods available */
export enum GMCMethod {
  ORB,
  ECC,
  SOF,
}

/**
 * BoTSORT Configuration
 */
export interface BoTSORTConfig {
  /**
   * if true, Global Motion Compensation is enabled
   * @default false
   */
  enable_gmc?: boolean;

  /**
   * confidence threshold to classify a detection as high confidence detection.
   * These detections are used in 1st level of association and to confirm a track
   *
   * @default 0.6
   */
  track_high_thresh?: number;

  /**
   * lowest possible confidence to use a detection in the tracking algo.
   * Any detection having confidence below this threshold is discarded
   * @default 0.1
   */
  track_low_thresh?: number;

  /**
   *confidence threshold to start a new track
   * @default 0.7
   */
  new_track_thresh?: number;

  /**
   * number governs the number of frames a track is kept alive without any detection.
   * max_alive_age = frame_rate / 30.0 * track_buffer
   * @default 30
   */
  track_buffer?: number;

  /**
   *cost threshold to match a detection to a track (iou + embedding distance), only used in 1st level of association
   * @default 0.7
   */
  match_thresh?: number;

  /**
   * IoU distance (1 - IoU) threshold to reject a detection. If a detection <-> track box IoU distance is greater than this threshold, the match is rejected
   * @default 0.5
   */
  proximity_thresh?: number;

  /**
   * frame rate of the video being processed
   * @default 30
   */
  frame_rate?: number;

  /**
   * GMC algorithim method to use
   * @deprecated
   */
  gmc_method?: GMCMethod;
}

/**
 * ORB GMC algorithim method
 *
 * @param
 */
export interface OrbGMCConfig {
  /**
   * GMC_method name
   */
  gmc_method: GMCMethod.ORB;

  /**
   * Factor to reduce image resolution
   * @default 2.0
   */
  downscale?: number;

  /**
   * @default 0.5
   */
  inlier_ratio?: number;

  /**
   * @default 0.99
   */
  ransac_conf?: number;

  /**
   * @default 1000
   */
  ransac_max_iters?: number;
}

export interface EccGMCConfig {
  /**
   * GMC_method name
   */
  gmc_method: GMCMethod.ECC;

  /**
   * Factor to reduce image resolution
   * @default 5.0
   */
  downscale?: number;

  /**
   * @default 500
   */
  max_iterations?: number;

  /**
   * @default 1000
   */
  termination_eps?: number;
}

export interface SofGMCConfig {
  /**
   * GMC_method name
   */
  gmc_method: GMCMethod.SOF;

  /**
   * Factor to reduce image resolution
   * @default 2.0
   */
  downscale?: number;

  /**
   * Enable the harris detector
   *
   * @default false
   */
  use_harris_detector?: boolean;

  /**
   * @default 1000
   */
  max_corners?: number;

  /**
   * @default 3
   */
  block_size?: number;

  /**
   * @default 0.01
   */
  quality_level?: number;

  /**
   * @default 0.04
   */
  k?: number;

  /**
   * @default 1.0
   */
  min_distance?: number;

  /**
   * @default 0.5
   */
  inlier_ratio?: number;

  /**
   * @default 0.99
   */
  ransac_conf?: number;

  /**
   * @default 500
   */
  ransac_max_iters?: number;
}

export type GMCConfig = OrbGMCConfig | EccGMCConfig | SofGMCConfig;

export interface BoTSortTracker extends HybridObject<{
  ios: 'c++';
  android: 'c++';
}> {
  initialize(trackerConfig: BoTSORTConfig, gmcConfig?: GMCConfig): void;
  track(frame: Frame, detections: BoundingBox[]): TrackedObject[];
}
