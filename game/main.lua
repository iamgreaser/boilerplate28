--[[
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
]]

D = {}
function D.ellipse(cx, cy, rx, ry, lcount)
	local l = {}
	local i

	l[#l+1] = cx
	l[#l+1] = cy
	for i=1,lcount+1 do
		local a = i*math.pi*2/lcount
		local x = cx + math.sin(a)*rx
		local y = cy + math.cos(a)*ry
		l[#l+1] = x
		l[#l+1] = y
	end

	return blob.new(GL.TRIANGLE_FAN, 2, l)
end

print("honk")
bl_test1 = D.ellipse(0, 0, 0.5-0.0, 0.3-0.0, 50)
bl_test2 = D.ellipse(0, 0, 0.5-0.03, 0.3-0.03, 50)

m_prj = M.new()
m_cam = M.new()
m_camtmp = M.new()

local sw, sh = sys.get_screen_dims()
M.scale(m_prj, sh/sw, 1.0, 1.0)
M.translate(m_cam, 0.0, 0.0, -1.0)

function hook_tick(sec_current, sec_delta)
	print("tick", sec_current, sec_delta)
end

blob_pos = {}
local i
for i = 1,300 do
	blob_pos[i] = {
		x = math.random()*2-1,
		y = math.random()*2-1,
		s = math.random()*0.8+0.2,
		ang = math.random() * math.pi * 2,
	}
end

function hook_render(sec_current, sec_delta)
	print("render", sec_current, sec_delta)

	M.rotate(m_cam, sec_delta, 0, 0, 1)

	M.load_projection(m_prj)
	M.load_modelview (m_cam)

	GL.glClearColor(0, ((math.sin(math.pi*sec_current)+1)/2) * 0.5, 1, 1)
	GL.glClear(GL.COLOR_BUFFER_BIT + GL.DEPTH_BUFFER_BIT + GL.STENCIL_BUFFER_BIT)

	-- Begin actual rendering!

	local i
	for i=1,#blob_pos do
		local bp = blob_pos[i]
		M.dup(m_camtmp, m_cam)
		M.identity(m_camtmp)
		M.translate(m_camtmp, bp.x, bp.y, 0)
		M.scale(m_camtmp, bp.s, bp.s, 1)
		M.rotate(m_camtmp, bp.ang, 0, 0, 1)
		M.apply(m_camtmp, m_cam)
		M.load_modelview (m_camtmp)
		blob.render(bl_test1, 0, 0, 0)
		blob.render(bl_test2, 1, 0, 1)
	end
end

