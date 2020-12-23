Script.lifetime = 12000
Script.fadetime = 4000

function Script:Start()
	self.fadestart = Time:GetCurrent() + self.lifetime - self.fadetime
end

function Script:UpdateWorld()
	local currenttime = Time:GetCurrent()
	if currenttime>self.fadestart then
		local alpha = 1.0 - (currenttime - self.fadestart) / self.fadetime
		if (alpha<=0) then
			self.entity:Release()
		else
			local color = self.entity:GetColor()
			self.entity:SetColor(color.r,color.g,color.b,alpha)
		end
	end
end
