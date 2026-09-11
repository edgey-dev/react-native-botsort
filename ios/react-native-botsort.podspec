require 'json'

package = JSON.parse(File.read(File.join(__dir__, "package.json")))

# --- Runs at podspec evaluation time (NOT skipped for :path pods, unlike prepare_command) ---
# This mirrors the fix pattern used by react-native-quick-crypto for the same
# CocoaPods :path/prepare_command gap: https://github.com/margelo/react-native-quick-crypto/pull/895

root = __dir__
opencv_xcframework = File.join(root, "opencv-mobile.xcframework")
motcpp_src = File.join(root, "motcpp-src")
motcpp_lib = File.join(motcpp_src, "build", "libmotcpp.a")

def run!(cmd)
  puts "[react-native-nitro-botsort] #{cmd}"
  system(cmd) || raise("[react-native-nitro-botsort] command failed: #{cmd}")
end

unless File.directory?(opencv_xcframework)
  Dir.chdir(root) do
    opencv_version = "v36"
    opencv_pkg = "opencv-mobile-2.4.13.7"

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

unless File.exist?(motcpp_lib)
  Dir.chdir(root) do
    run!("git clone --depth 1 --branch main https://github.com/Geekgineer/motcpp.git motcpp-src") unless File.directory?(motcpp_src)
    run!("cmake -S motcpp-src -B motcpp-src/build " \
         "-DCMAKE_SYSTEM_NAME=iOS " \
         "-DCMAKE_OSX_DEPLOYMENT_TARGET=15.1 " \
         "-DCMAKE_BUILD_TYPE=Release " \
         "-DMOTCPP_BUILD_TESTS=OFF")
    run!("cmake --build motcpp-src/build --config Release -j$(sysctl -n hw.ncpu)")
  end
end

Pod::Spec.new do |s|
  s.name         = "react-native-botsort"
  s.version      = package["version"]
  s.summary      = "BoT-SORT Multi-Object Tracking Engine"
  s.homepage     = "https://github.com/<your-org>/react-native-nitro-botsort"
  s.license      = { :type => "AGPL-3.0", :text => "See LICENSE — this pod links motcpp (AGPL-3.0); combined-work obligations apply to consumers." }
  s.authors      = { "Developer" => "dev@domain.com" }
  s.platforms    = { :ios => "15.1" }
  s.source       = { :git => "https://github.com/<your-org>/react-native-nitro-botsort.git", :tag => "#{s.version}" }

  s.source_files = "ios/**/*.{h,m,mm,swift}",
                    "cpp/**/*.{hpp,cpp}",
                    "nitrogen/generated/ios/**/*.{h,m,mm,hpp,cpp}"

  s.dependency "react-native-nitro-modules"
  s.dependency "onnxruntime-c", "~> 1.18.0"
  s.dependency "Eigen", "~> 3.4"
  s.dependency "yaml-cpp"

  s.vendored_frameworks = "opencv-mobile.xcframework"
  s.vendored_libraries  = motcpp_lib
  s.header_mappings_dir = "motcpp-src/include"

  s.pod_target_xcconfig = {
    "CLANG_CXX_LANGUAGE_STANDARD" => "c++17",
    "HEADER_SEARCH_PATHS" => '"$(PODS_TARGET_SRCROOT)/../cpp" "$(PODS_TARGET_SRCROOT)/motcpp-src/include"',

    "GCC_OPTIMIZATION_LEVEL" => "3",
    "LLVM_LTO" => "YES",
    "GCC_SYMBOLS_PRIVATE_EXTERN" => "YES",
    "DEPLOYMENT_POSTPROCESSING" => "YES",
    "STRIP_INSTALLED_PRODUCT" => "YES",
    "STRIP_STYLE" => "all",
    "DEAD_CODE_STRIPPING" => "YES"
  }
end