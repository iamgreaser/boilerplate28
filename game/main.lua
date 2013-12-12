--[[
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
]]

print("honk")
btest = blob.new(2, {
	1,0,
	0,1,
	-1,0,
	0,-1,
})
print("blob", btest)

function hook_tick(sec_current, sec_delta)
	print("tick", sec_current, sec_delta)
end

function hook_render(sec_current, sec_delta)
	print("render", sec_current, sec_delta)

	m = M.new()
	M.load_projection(m)
	M.load_modelview(m)

	GL.glClearColor(sec_current % 1.0, 0, 1, 1)
	GL.glClear(GL.COLOR_BUFFER_BIT + GL.DEPTH_BUFFER_BIT + GL.STENCIL_BUFFER_BIT)
end

