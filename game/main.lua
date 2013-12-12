--[[
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
]]

dofile("game/lib_sdlkey.lua")

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

bl_test1 = D.ellipse(0, 0, 0.2-0.0, 0.3-0.0, 50)

m_prj = M.new()
m_cam = M.new()
m_camtmp = M.new()

local sw, sh = sys.get_screen_dims()
M.scale(m_prj, sh/sw, 1.0, 1.0)
M.translate(m_cam, 0.0, 0.0, -1.0)

function hook_tick(sec_current, sec_delta)
	--print("tick", sec_current, sec_delta)
	print("mouse", sys.get_mouse())
end

blob_pos = {}
local i
for i = 1,300 do
	blob_pos[i] = {
		x = math.random()*2-1,
		y = math.random()*2-1,
		s = math.random()*0.3+0.2,
		ang = math.random() * math.pi * 2,
		angspd = math.random() * 3.0 + 1.0,
	}
end

mouse_x, mouse_y = nil, nil
mouse_drag = false

function hook_click(sec_current, mx, my, button, state)
	print("click", sec_current, mx, my, button, state)
end

function hook_key(sec_current, mod, key, state)
	print("key", sec_current, mod, key, state)
end

function hook_render(sec_current, sec_delta)
	--print("render", sec_current, sec_delta)

	M.load_projection(m_prj)
	M.load_modelview (m_cam)

	GL.glClearColor(0, ((math.sin(math.pi*sec_current)+1)/2) * 0.5, 1, 1)
	GL.glClear(GL.COLOR_BUFFER_BIT + GL.DEPTH_BUFFER_BIT + GL.STENCIL_BUFFER_BIT)

	-- Begin actual rendering!

	local i, j
	for j=1,3 do
		for i=1,#blob_pos do
			local bp = blob_pos[i]
			M.dup(m_camtmp, m_cam)
			M.identity(m_camtmp)
			M.translate(m_camtmp, bp.x, bp.y, 0)
			if j == 1 then M.scale(m_camtmp, bp.s+0.2, bp.s+0.2, 1)
			elseif j == 2 then M.scale(m_camtmp, bp.s, bp.s, 1)
			else M.scale(m_camtmp, bp.s-0.15, bp.s-0.15, 1)
			end
			M.rotate(m_camtmp, bp.ang, 0, 0, 1)
			M.apply(m_camtmp, m_cam)
			M.load_modelview(m_camtmp)
			if j == 1 then blob.render(bl_test1, 0, 0, 0)
			elseif j == 2 then blob.render(bl_test1, 1, 0, 1)
			else blob.render(bl_test1, 1, 0.5, 1)
			end

			if j == 2 then
			bp.ang = bp.ang + bp.angspd*sec_delta
			end
		end
	end
end

