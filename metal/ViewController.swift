import Cocoa

import Metal
import MetalKit

class ViewController: NSViewController
{
    var mtkView: MTKView!

    override func viewDidLoad()
    {
        super.viewDidLoad()

        guard let mtkViewTemp = self.view as? MTKView else
        {
            print("View attached to ViewController is not an MTKView!")
            return
        }
        mtkView = mtkViewTemp

        guard let defaultDevice = MTLCreateSystemDefaultDevice() else
        {
            print("Metal is not supported on this device")
            return
        }

        print("My GPU is: \(defaultDevice)")
        mtkView.device = defaultDevice
    }
}
