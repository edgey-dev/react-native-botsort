require "json"

package = JSON.parse(File.read(File.join(__dir__, "package.json")))

eigen_dir = File.join(__dir__, "eigen-3.4.0")
unless File.directory?(File.join(eigen_dir, "Eigen"))
  Dir.chdir(__dir__) do
    archive = "eigen-3.4.0.tar.gz"
    system("curl", "-sSfL", "-o", archive,
           "https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz") ||
      raise("[NitroBotsort] Failed to download Eigen 3.4.0")
    system("tar", "-xzf", archive) ||
      raise("[NitroBotsort] Failed to extract Eigen 3.4.0")
    File.delete(archive)
  end
end

ios_dir = File.join(__dir__, "ios")
opencv_dir = File.join(ios_dir, "opencv-mobile")
unless File.directory?(File.join(opencv_dir, "include", "opencv4", "opencv2")) &&
       !Dir.glob(File.join(opencv_dir, "lib", "*.a")).empty?
  Dir.chdir(ios_dir) do
    archive = "ios.zip"
    system("curl", "-sSfL", "-o", archive,
           "https://github.com/edgey-dev/react-native-botsort/releases/download/opencv-botsort-4.12.0/opencv-mobile-4.12.0-ios.zip") ||
      raise("[NitroBotsort] Failed to download OpenCV for iOS")
    system("unzip", "-q", "-o", archive, "-d", "opencv-mobile") ||
      raise("[NitroBotsort] Failed to extract OpenCV for iOS")
    File.delete(archive)
  end
end

Pod::Spec.new do |s|
  s.name         = "NitroBotsort"
  s.version      = package["version"]
  s.summary      = package["description"]
  s.homepage     = package["homepage"]
  s.license      = package["license"]
  s.authors      = package["author"]

  s.platforms    = { :ios => min_ios_version_supported, :visionos => 1.0 }
  s.source       = { :git => "https://github.com/margelo/nitro.git", :tag => "#{s.version}" }

  s.source_files = [
    # Implementation (Swift)
    "ios/**/*.{swift}",
    # Autolinking/Registration (Objective-C++)
    "ios/**/*.{m,mm}",
    # Implementation (C++ objects)
    "cpp/**/*.{hpp,cpp}",
  ]

  load 'nitrogen/generated/ios/NitroBotsort+autolinking.rb'
  add_nitrogen_files(s)

  s.dependency 'React-jsi'
  s.dependency 'React-callinvoker'
  s.dependency 'VisionCamera'
  install_modules_dependencies(s)

  s.vendored_libraries = 'ios/opencv-mobile/lib/*.a'

  current_pod_target_xcconfig = s.attributes_hash['pod_target_xcconfig'] || {}
  s.pod_target_xcconfig = current_pod_target_xcconfig.merge({
    'HEADER_SEARCH_PATHS' => '"$(inherited)" "$(PODS_TARGET_SRCROOT)/cpp" "$(PODS_TARGET_SRCROOT)/eigen-3.4.0" "$(PODS_TARGET_SRCROOT)/ios/opencv-mobile/include/opencv4"'
  })
end
