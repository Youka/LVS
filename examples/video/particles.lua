-- Particle objects storage
local particles = {n = 0}

-- Particle image source
local particle_source
do
	local particle_image = g2d.create_image("RGBA", 22, 22)
	local ctx = g2d.create_context(particle_image)
	ctx:path_move_to(1,11)
	ctx:path_curve_to(11,11,11,11,11,1)
	ctx:path_curve_to(11,11,11,11,21,11)
	ctx:path_curve_to(11,11,11,11,11,21)
	ctx:path_curve_to(11,11,11,11,1,11)
	ctx:set_source(g2du.white)
	ctx:path_fill()
	particle_source = g2d.create_source_image(g2d.image_convolution(particle_image, g2du.create_gaussian_blur_kernel(1)))
end

-- Particles way
local curve_points = {
	{100, 100},
	{400, 400},
	{700, -200},
	{700, 500},
	{-200, 500},
	{100,100}
}

-- Particle objects update
local function update(pct)
	-- Move and age particle objects
	local actual_particles = {n = 0}
	for _, particle in ipairs(particles) do
		particle.life = particle.life - 1
		if particle.life > 0 then
			particle.cur_pos[1] = particle.cur_pos[1] + particle.dir[1]
			particle.cur_pos[2] = particle.cur_pos[2] + particle.dir[2]
			actual_particles.n = actual_particles.n + 1
			actual_particles[actual_particles.n] = particle
		end
	end
	particles = actual_particles
	-- Calculate particles way point
	local pos = math.bezier(pct, curve_points)
	-- Add new particle objects
	for i=1, 20 do
		particles.n = particles.n + 1
		particles[particles.n] = {
			cur_pos = {pos[1] + math.random(0,10) * 0.5, pos[2] + math.random(0,10) * 0.5},
			dir = {math.random(-10,10) * 0.5, math.random(-10,10) * 0.5},
			life = 5,
			max_life = 5
		}
	end
end

-- Visualizes particle objects
local function draw(ctx)
	-- Iterate through particle objects
	for _, particle in ipairs(particles) do
		-- Set particle position
		ctx:set_matrix(g2d.create_matrix():translate(particle.cur_pos[1], particle.cur_pos[2]))
		-- Set particle source
		ctx:set_source(particle_source)
		-- Paint fading particle
		ctx:masked_paint(g2d.create_source_color(1,1,1,particle.life / particle.max_life))
	end
end

-- Process frames
function GetFrame(frame, frame_i)
	-- Update particle objects
	update(frame_i % 101 / 100)
	-- Draw particles
	draw(g2d.create_context(frame))
end