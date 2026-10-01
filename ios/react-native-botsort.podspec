require 'json'

package = JSON.parse(File.read(File.join(__dir__, "package.json")))

# --- Runs at podspec evaluation time (NOT skipped for :path pods, unlike prepare_command) ---
# This mirrors the fix pattern used by react-native-quick-crypto for the same
# CocoaPods :path/prepare_command gap: https://github.com/margelo/react-native-quick-crypto/pull/895

root = __dir__
opencv_xcframework = File.join(root, "opencv-mobile.xcframework")

def run!(cmd)
  puts "[react-native-botsort] #{cmd}"
  system(cmd) || raise("[react-native-botsort] command failed: #{cmd}")
end

unless File.directory?(opencv_xcframework)
  Dir.chdir(root) do

    run!("curl -sSfL -o ios.zip https://github.com/edgey-dev/react-native-botsort/releases/download/opencv-botsort-4.12.0/opencv-mobile-4.12.0-ios.zip")
    run!("unzip -q -o ios.zip -d ios_device")
    run!("xcodebuild -create-xcframework " \
         "-framework ios_device/opencv2.framework " \
         "-output #{opencv_xcframework}")
    run!("rm -rf ios.zip ios_device")
  end
end

Pod::Spec.new do |s|
  s.name         = "react-native-botsort"
  s.version      = package["version"]
  s.summary      = "BoT-SORT Multi-Object Tracking Engine"
  s.homepage     = "https://github.com/edgey-dev/react-native-botsort"
  s.license      = { :type => "MIT", :text => "See LICENSE." }
  s.authors      = { "Developer" => "juwonchina@gmail.com" }
  s.platforms    = { :ios => "15.1" }
  s.source       = { :git => "https://github.com/edgey-dev/react-native-botsort.git", :tag => "#{s.version}" }

  s.source_files = "ios/**/*.{h,m,mm,swift}",
                    "cpp/**/*.{hpp,cpp}",
                    "nitrogen/generated/ios/**/*.{h,m,mm,hpp,cpp}"

  s.dependency "react-native-nitro-modules"
  s.dependency "react-native-vision-camera"
  s.dependency "Eigen", "~> 3.4"

  s.vendored_frameworks = "opencv-mobile.xcframework"

  s.pod_target_xcconfig = {
    "CLANG_CXX_LANGUAGE_STANDARD" => "c++20",
    "HEADER_SEARCH_PATHS" => '"$(inherited)" "$(PODS_TARGET_SRCROOT)/../cpp" "$(PODS_ROOT)/Headers/Public" "$(PODS_ROOT)/Headers/Public/eigen3" "$(PODS_ROOT)/eigen3" "$(PODS_ROOT)/eigen3/Eigen"',

    "GCC_OPTIMIZATION_LEVEL" => "3",
    "LLVM_LTO" => "YES",
    "GCC_SYMBOLS_PRIVATE_EXTERN" => "YES",
    "DEPLOYMENT_POSTPROCESSING" => "YES",
    "STRIP_INSTALLED_PRODUCT" => "YES",
    "STRIP_STYLE" => "all",
    "DEAD_CODE_STRIPPING" => "YES"
  }
end