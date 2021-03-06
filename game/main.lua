--[[
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
]]

dofile("game/lib_sdlkey.lua")

img_font = png.load("dat/font.png")
w_splat1 = wav.load("dat/splat1.wav")
m_lp64 = mus.load("dat/lp64-munch.it")
mus.play(m_lp64)
m_lp64 = nil

D = {}
function D.eqpoly(cx, cy, r, pcount)
	local l = {}
	local i

	l[#l+1] = cx
	l[#l+1] = cy
	for i=1,pcount+1 do
		local a = i*math.pi*2/pcount
		local x = cx + math.sin(a)*r
		local y = cy + math.cos(a)*r
		l[#l+1] = x
		l[#l+1] = y
	end

	return blob.new(GL.TRIANGLE_FAN, 2, l)
end

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

function D.star(cx, cy, r1, r2, pcount)
	local l = {}
	local i

	l[#l+1] = cx
	l[#l+1] = cy
	local as = math.pi*2/pcount
	for i=1,pcount+1 do
		local a = i*as
		local x = cx + math.sin(a)*r1
		local y = cy + math.cos(a)*r1
		l[#l+1] = x
		l[#l+1] = y

		if i ~= pcount+1 then
			local x = cx + math.sin(a+as/2)*r2
			local y = cy + math.cos(a+as/2)*r2
			l[#l+1] = x
			l[#l+1] = y
		end
	end

	return blob.new(GL.TRIANGLE_FAN, 2, l)
end

R = {}
function R.text(x, y, s, sx, sy, r, g, b, a)
	sx = sx or 1
	sy = sy or sx
	r = r or 1
	g = g or 1
	b = b or 1
	a = a or 1
	local i
	for i=1,#s do
		local tx = (s:byte(i) - 32)/96
		png.render(img_font, x, y, x + 6*sx, y + 8*sy, tx, 0, tx + 5.5/(96*6), 1,
			r, g, b, a)
		x = x + 6*sx
	end
end
function R.text_s(o, x, y, s, sx, sy, r, g, b, a)
	a = a or 1
	R.text(x + o, y, s, sx, sy, 0, 0, 0, a*0.7)
	R.text(x - o, y, s, sx, sy, 0, 0, 0, a*0.7)
	R.text(x, y + o, s, sx, sy, 0, 0, 0, a*0.7)
	R.text(x, y - o, s, sx, sy, 0, 0, 0, a*0.7)
	R.text(x, y, s, sx, sy, r, g, b, a)
end

--bl_test1 = D.ellipse(0, 0, 0.2-0.0, 0.3-0.0, 50)
--bl_test1 = D.star(0, 0, 0.30, 0.15, 5)
--bl_test1 = D.eqpoly(0, 0, 0.30, 3)

bl_test1 = blob.new(GL.TRIANGLE_STRIP, 2, {
		-0.04, -0.1,
		 0.04, -0.1,
		-0.04,  0.7,
		 0.04,  0.7,
	})
bl_test2 = D.eqpoly(0, 0, 0.30, 30)

m_prj = M.new()
m_cam = M.new()
m_camtmp = M.new()
m_prj_hud = M.new()
m_cam_hud = M.new()

local sw, sh = sys.get_screen_dims()
M.scale(m_prj, sh/sw, 1.0, 1.0)
M.translate(m_cam, 0.0, 0.0, -1.0)

blob_palette = {
	{1, 0, 0},
	{1, 0.5, 0},
	{1, 1, 0},
	{0, 1, 0},
}
blob_pos = {}
local i
for i = 1,300 do
	local c = math.floor(math.random()*#blob_palette)+1
	blob_pos[i] = {
		bx = math.random()*2-1,
		by = math.random()*2-1,
		s = math.random()*0.3+0.2,
		ang = math.random() * math.pi * 2,
		angspd = math.random() * 3.0 + 1.0,
		r = blob_palette[c][1],
		g = blob_palette[c][2],
		b = blob_palette[c][3],
	}
	blob_pos[i].x = blob_pos[i].bx
	blob_pos[i].y = blob_pos[i].by
end

mouse_x, mouse_y = nil, nil
mouse_drag = false
mouse_b_left = false
mouse_b_middle = false
mouse_b_right = false

function hook_click(sec_current, mx, my, button, state)
	print("click", sec_current, mx, my, button, state)
	if button == 1 then mouse_b_left = state
	elseif button == 2 then mouse_b_middle = state
	elseif button == 3 then mouse_b_right = state
	end

	if state and w_splat1 ~= nil then
		wav.play(w_splat1, 1.0, 1.0, 1 + 0.3*(math.random()*2-1))
	end
end

function hook_key(sec_current, mod, key, state)
	print("key", sec_current, mod, key, state)
	if state and key == SDLK_F1 then
		m_lp64 = mus.load("dat/lp64-munch.it")
		mus.play(m_lp64)
		m_lp64 = nil
	elseif state and key == SDLK_F2 then
		m_lp64 = mus.load("dat/lp64-munch.it")
		mus.play(m_lp64)
		m_lp64 = nil
		collectgarbage()
	elseif state and key == SDLK_F3 then
		mus.stop()
	end
end

local lspeed = 3.0
function hook_tick(sec_current, sec_delta)
	local sw, sh = sys.get_screen_dims()
	local mx, my = sys.get_mouse()
	local is_focused = (mx ~= -1)
	mx = (mx - (sw/2)) / (sh/2)
	my = -(my - (sh/2)) / (sh/2)
	for i=1,#blob_pos do
		local bp = blob_pos[i]

		-- move to base x
		bp.x = bp.x + (bp.bx - bp.x) * (1.0 - math.exp(-sec_delta*lspeed))
		bp.y = bp.y + (bp.by - bp.y) * (1.0 - math.exp(-sec_delta*lspeed))

		-- move away from mouse
		if is_focused then
			local dx = bp.x - mx
			local dy = bp.y - my
			local dd = math.sqrt(dx*dx + dy*dy)
			if mouse_b_left then dd = -dd end
			dx = dx / dd
			dy = dy / dd
			bp.x = bp.x + dx*sec_delta
			bp.y = bp.y + dy*sec_delta
		end
	end
end

fps_current = 1
function hook_render(sec_current, sec_delta)
	--print("render", sec_current, sec_delta)
	local sw, sh = sys.get_screen_dims()

	M.load_projection(m_prj)
	M.load_modelview (m_cam)

	GL.glClearColor(0, ((math.sin(math.pi*sec_current)+1)/2) * 0.5, 1, 1)
	GL.glClear(GL.COLOR_BUFFER_BIT + GL.DEPTH_BUFFER_BIT + GL.STENCIL_BUFFER_BIT)

	-- Begin actual rendering!

	local i, j
	for j=1,6 do
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
			
			--[[
			if j == 1 then blob.render(bl_test1, 0, 0, 0)
			elseif j == 2 then blob.render(bl_test1, 0.9, 0.6, 0)
			else blob.render(bl_test1, 1, 1, 0)
			end
			]]
			if j == 1 then blob.render(bl_test1, 0, 0, 0)
			elseif j == 2 then blob.render(bl_test2, 0, 0, 0)
			elseif j == 3 then blob.render(bl_test1, 0.7, 0.7, 0.7)
			elseif j == 4 then blob.render(bl_test1, 1, 1, 1)
			elseif j == 5 then blob.render(bl_test2, bp.r, bp.g, bp.b, 0.5)
			elseif j == 6 then blob.render(bl_test2, bp.r, bp.g, bp.b, 0.5)
			end

			if j == 6 then
			bp.ang = bp.ang + bp.angspd*sec_delta
			end
		end
	end

	M.identity(m_cam_hud)
	M.scale(m_cam_hud, 1, -1, 1)
	M.translate(m_cam_hud, -1, -1, -1)
	M.scale(m_cam_hud, 2/sw, 2/sh, 1)
	M.load_projection(m_prj_hud)
	M.load_modelview (m_cam_hud)

	local iw, ih = png.get_dims(img_font)
	R.text_s(3, 100, 100, "You're a LOLLIPOP!", 6, 6,
		math.sin(5*sec_current),
		math.sin(5*sec_current + math.pi*2/3),
		math.sin(5*sec_current + math.pi*4/3),
		0.7)
	
	local fps_calc = 1.0/sec_delta
	fps_current = fps_current + (fps_calc - fps_current) * 0.1
	R.text_s(1, 8, sh-16-8, string.format("%.2f FPS", fps_current), 2, 2)
end

