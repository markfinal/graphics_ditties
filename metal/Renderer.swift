import Metal
import MetalKit

extension Double
{
    static func random() -> Double
    {
        return Double(arc4random()) / Double(UInt32.max)
    }
}

class Renderer : NSObject, MTKViewDelegate
{
    let device: MTLDevice
    let commandQueue: MTLCommandQueue

    // This is the initializer for the Renderer class.
    // We will need access to the mtkView later, so we add it as a parameter here.
    init?(mtkView: MTKView)
    {
        device = mtkView.device!

        commandQueue = device.makeCommandQueue()!
    }

    // mtkView will automatically call this function
    // whenever it wants new content to be rendered.
    func draw(in view: MTKView)
    {
        // Get an available command buffer
        guard let commandBuffer = commandQueue.makeCommandBuffer() else { return }

        // Get the default MTLRenderPassDescriptor from the MTKView argument
        guard let renderPassDescriptor = view.currentRenderPassDescriptor else { return }

        // Change default settings. For example, we change the clear color from black to a random color.
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(Double.random(), Double.random(), Double.random(), 1)

        // We compile renderPassDescriptor to a MTLRenderCommandEncoder.
        guard let renderEncoder = commandBuffer.makeRenderCommandEncoder(descriptor: renderPassDescriptor) else { return }

        // TODO: Here is where we need to encode drawing commands!

        // This finalizes the encoding of drawing commands.
        renderEncoder.endEncoding()

        // Tell Metal to send the rendering result to the MTKView when rendering completes
        commandBuffer.present(view.currentDrawable!)

        // Finally, send the encoded command buffer to the GPU.
        commandBuffer.commit()
    }

    // mtkView will automatically call this function
    // whenever the size of the view changes (such as resizing the window).
    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize)
    {
    }
}
