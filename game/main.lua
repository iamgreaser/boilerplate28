--[[
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
]]

print("honk")
bl_test = blob.new(GL.TRIANGLE_FAN, 2, {
	0,0,
	1,0,
	0,1,
	-1,0,
	0,-1,
	1,0,
})

m_prj = M.new()
m_cam = M.new()

local sw, sh = sys.get_screen_dims()
M.scale(m_prj, sh/sw, 1.0, 1.0)
M.translate(m_cam, 0.0, 0.0, -1.0)

function hook_tick(sec_current, sec_delta)
	print("tick", sec_current, sec_delta)
end

function hook_render(sec_current, sec_delta)
	print("render", sec_current, sec_delta)

	M.rotate(m_cam, sec_delta, 0, 0, 1)

	M.load_projection(m_prj)
	M.load_modelview (m_cam)

	GL.glClearColor(0, ((math.sin(math.pi*sec_current)+1)/2) * 0.5, 1, 1)
	GL.glClear(GL.COLOR_BUFFER_BIT + GL.DEPTH_BUFFER_BIT + GL.STENCIL_BUFFER_BIT)

	-- Begin actual rendering!

	blob.render(bl_test, 1, 0, 1)
end

