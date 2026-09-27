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
    opencv_version = "v34"
    opencv_pkg = "opencv-mobile-4.12.0"

    run!("curl -sSfL -o ios.zip https://github.com/nihui/opencv-mobile/releases/download/#{opencv_version}/#{opencv_pkg}-ios.zip")
    run!("curl -sSfL -o ios-sim.zip https://github.com/nihui/opencv-mobile/releases/download/#{opencv_version}/#{opencv_pkg}-ios-simulator.zip")
    run!("unzip -q -o ios.zip -d ios_device")
    run!("unzip -q -o ios-sim.zip -d ios_simulator")
    run!("xcodebuild -create-xcframework " \
         "-framework ios_device/opencv2.framework " \
         "-framework ios_simulator/opencv2.framework " \
         "-output #{opencv_xcframework}")
    run!("rm -rf ios.zip ios-sim.zip ios_device ios_simulator")
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
    "HEADER_SEARCH_PATHS" => '"$(PODS_TARGET_SRCROOT)/../cpp"',

    "GCC_OPTIMIZATION_LEVEL" => "3",
    "LLVM_LTO" => "YES",
    "GCC_SYMBOLS_PRIVATE_EXTERN" => "YES",
    "DEPLOYMENT_POSTPROCESSING" => "YES",
    "STRIP_INSTALLED_PRODUCT" => "YES",
    "STRIP_STYLE" => "all",
    "DEAD_CODE_STRIPPING" => "YES"
  }
end